#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* CBMC nondeterministic helpers */
size_t nondet_size_t(void);
void *nondet_void_ptr(void);

void aws_array_list_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic item size, length and capacity */
    size_t item_size = nondet_size_t();
    size_t length = nondet_size_t();
    size_t capacity = nondet_size_t();

    /* reasonable bounds to avoid overflow in the harness */
    __CPROVER_assume(item_size > 0 && item_size <= 64);
    __CPROVER_assume(length <= 8);
    __CPROVER_assume(capacity <= 8);
    __CPROVER_assume(capacity >= length);

    /* compute current_size = capacity * item_size, avoid overflow */
    size_t current_size = capacity * item_size;
    __CPROVER_assume(current_size / item_size == capacity); /* no overflow */

    /* allocate data buffer if capacity > 0 */
    uint8_t *data = NULL;
    if (capacity > 0) {
        data = malloc(current_size);
        __CPROVER_assume(data != NULL);
        /* initialize with nondeterministic bytes */
        for (size_t i = 0; i < current_size; ++i) {
            data[i] = nondet_uint8_t();
        }
    }

    /* initialize list fields */
    list.alloc = alloc;
    list.data = data;
    list.item_size = item_size;
    list.length = length;
    list.current_size = current_size;

    /* ensure the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* snapshot list state for frame condition checks */
    uint8_t *data_snapshot = NULL;
    if (current_size > 0) {
        data_snapshot = malloc(current_size);
        __CPROVER_assume(data_snapshot != NULL);
        memcpy(data_snapshot, data, current_size);
    }
    size_t length_snapshot = list.length;
    size_t current_size_snapshot = list.current_size;
    void *alloc_snapshot = list.alloc;

    /* allocate output buffer */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    /* initialize output buffer with nondet data */
    for (size_t i = 0; i < item_size; ++i) {
        ((uint8_t *)val)[i] = nondet_uint8_t();
    }

    /* call the function under test */
    int ret = aws_array_list_front(&list, val);

    /* postconditions */

    /* 1. Return value / error code correctness */
    if (length_snapshot > 0) {
        /* success case */
        assert(ret == AWS_OP_SUCCESS);
        /* output buffer must contain the first element */
        assert(memcmp(val, data_snapshot, item_size) == 0);
    } else {
        /* error case: list empty */
        assert(ret != AWS_OP_SUCCESS);
    }

    /* 2. Output buffer length/capacity invariants */
    /* (already covered by the memcpy check above) */

    /* 3. Memory not modified beyond the function's contract (frame conditions) */
    /* list structure fields must remain unchanged */
    assert(list.alloc == alloc_snapshot);
    assert(list.item_size == item_size);
    assert(list.length == length_snapshot);
    assert(list.current_size == current_size_snapshot);
    /* data buffer must be unchanged */
    if (current_size_snapshot > 0) {
        assert(memcmp(list.data, data_snapshot, current_size_snapshot) == 0);
    }

    /* list must still be valid */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(data);
    free(data_snapshot);
    free(val);

    return 0;
}
