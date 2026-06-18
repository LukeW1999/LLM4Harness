#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U

void aws_array_list_get_at_ptr_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondet capacity and item size within bounds */
    size_t init_capacity = __CPROVER_nondet_size_t();
    __CPROVER_assume(init_capacity <= MAX_INITIAL_ITEM_ALLOCATION);
    size_t item_size = __CPROVER_nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* initialize a valid list */
    aws_array_list_init(&list, allocator, init_capacity, item_size);

    /* nondet length respecting capacity */
    size_t length = __CPROVER_nondet_size_t();
    __CPROVER_assume(length <= init_capacity);
    /* if length > 0 then data must be non‑NULL (ensured by init when capacity>0) */
    __CPROVER_assume(!(length > 0) || list.data != NULL);
    list.length = length;

    /* optionally fill the data buffer with nondet bytes */
    if (list.data != NULL && list.current_size > 0) {
        uint8_t *buf = (uint8_t *)list.data;
        size_t i;
        for (i = 0; i < list.current_size; ++i) {
            buf[i] = __CPROVER_nondet_uint8_t();
        }
    }

    /* snapshot of the whole list structure */
    struct aws_array_list old_list = list;

    /* snapshot of the underlying data buffer */
    uint8_t *old_data = NULL;
    if (list.data != NULL && list.current_size > 0) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data);
        memcpy(old_data, list.data, list.current_size);
    }

    void *val = NULL;
    void *old_val = val; /* snapshot for error case */

    size_t index = __CPROVER_nondet_size_t();

    int result = aws_array_list_get_at_ptr(&list, &val, index);

    /* ----- postconditions ----- */
    if (result == AWS_OP_SUCCESS) {
        assert(index < aws_array_list_length(&old_list));
        assert(val == (void *)((uint8_t *)old_list.data + old_list.item_size * index));
    } else {
        assert(index >= aws_array_list_length(&old_list));
        assert(val == old_val);
    }

    /* list invariants unchanged */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /* memory not modified beyond the function's contract */
    if (old_data != NULL) {
        assert(memcmp(old_data, list.data, list.current_size) == 0);
        free(old_data);
    }

    aws_array_list_clean_up(&list);
}
