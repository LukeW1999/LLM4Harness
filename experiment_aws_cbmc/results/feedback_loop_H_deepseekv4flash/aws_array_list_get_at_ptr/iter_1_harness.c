#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_get_at_ptr_harness() {
    /* Data structures */
    struct aws_array_list list;
    void **val;
    size_t index;

    /* Assumptions about the list */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Assumptions about val: must be a valid writable pointer to a void* */
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, sizeof(void *)));

    /* Save old state of list (const, but we will assert unchanged fields) */
    struct aws_array_list old_list = list;

    /* Call the function */
    int result = aws_array_list_get_at_ptr(&list, val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: the pointer should point to the expected element */
        assert(*val == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        /* Failure: val may be undefined; we only assert list unchanged and error set */
        assert(aws_last_error() == AWS_ERROR_INVALID_INDEX);
    }

    /* Unchanged fields: list is const, so all fields must be the same */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
