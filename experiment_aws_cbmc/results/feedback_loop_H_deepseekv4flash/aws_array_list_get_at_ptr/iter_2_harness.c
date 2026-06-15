#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_get_at_ptr_harness() {
    /* Data structures */
    struct aws_array_list list;
    void *local_val;
    void **val = &local_val;
    size_t index;

    /* Assumptions about the list */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state of list */
    struct aws_array_list old_list = list;

    /* Call the function */
    int result = aws_array_list_get_at_ptr(&list, val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, val should point to the element at index */
        __CPROVER_assert(*val == (void *)((uint8_t *)list.data + (list.item_size * index)),
                         "Success: *val points to element at index");
    } else {
        /* On failure, the error should be set appropriately */
        __CPROVER_assert(aws_last_error() == AWS_ERROR_INVALID_INDEX,
                         "Failure: error is AWS_ERROR_INVALID_INDEX");
    }

    /* Unchanged fields */
    __CPROVER_assert(list.alloc == old_list.alloc, "alloc unchanged");
    __CPROVER_assert(list.current_size == old_list.current_size, "current_size unchanged");
    __CPROVER_assert(list.length == old_list.length, "length unchanged");
    __CPROVER_assert(list.item_size == old_list.item_size, "item_size unchanged");
    __CPROVER_assert(list.data == old_list.data, "data unchanged");

    /* Validity invariant */
    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");
}
