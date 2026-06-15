#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Nondeterministic helpers for CBMC */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);
int nondet_int(void);

void aws_array_list_get_at_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Nondeterministic item size (must be > 0) */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    /* Nondeterministic initial allocation (number of items) */
    size_t init_items = nondet_size_t();
    __CPROVER_assume(init_items > 0);
    __CPROVER_assume(init_items <= 8); /* keep bounds reasonable for CBMC */

    /* Initialise the list in dynamic mode */
    int init_res = aws_array_list_init_dynamic(&list, allocator, init_items, item_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);

    /* Determine capacity from the allocated size */
    size_t capacity = list.current_size / list.item_size;
    __CPROVER_assume(capacity * list.item_size == list.current_size);
    __CPROVER_assume(list.current_size > 0);
    __CPROVER_assume(list.data != NULL);

    /* Nondeterministic length, must satisfy 0 <= length <= capacity */
    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);
    list.length = length;

    /* Fill the underlying buffer with nondeterministic data */
    for (size_t i = 0; i < list.current_size; ++i) {
        ((uint8_t *)list.data)[i] = nondet_uint8_t();
    }

    /* Allocate a writable output buffer */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Nondeterministic index to read */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= capacity); /* index may be out of bounds */

    /* Snapshot the list's data buffer to check frame condition later */
    uint8_t *data_snapshot = malloc(list.current_size);
    __CPROVER_assume(data_snapshot != NULL);
    memcpy(data_snapshot, list.data, list.current_size);

    /* Call the function under test */
    int ret = aws_array_list_get_at(&list, val, index);

    /* ---- Postcondition 1: return value / error code correctness ---- */
    if (index < list.length) {
        assert(ret == AWS_OP_SUCCESS);
    } else {
        assert(ret != AWS_OP_SUCCESS);
    }

    /* ---- Postcondition 2: output buffer correctness ---- */
    if (ret == AWS_OP_SUCCESS) {
        assert(0 == memcmp(val,
                           (uint8_t *)list.data + (list.item_size * index),
                           list.item_size));
    }

    /* ---- Postcondition 3: memory frame condition (no modification of list) ---- */
    assert(0 == memcmp(data_snapshot, list.data, list.current_size));

    /* Clean up */
    aws_array_list_clean_up(&list);
    free(val);
    free(data_snapshot);
}
