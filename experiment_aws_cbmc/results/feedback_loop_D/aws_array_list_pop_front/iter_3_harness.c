#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_pop_front_harness() {
    struct aws_array_list list;
    struct aws_array_list old_list;

    // Initialize the list with some values
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    // Ensure the list is not empty for the success case
    __CPROVER_assume(list.current_size > 0);

    // Save the old state
    old_list = list;

    int result = aws_array_list_pop_front(&list);

    // Check success path
    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size - 1);
        assert(list.length == old_list.length - 1);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    }
    // Check failure path
    else {
        assert(result == aws_raise_error(AWS_ERROR_LIST_EMPTY));
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    }

    // Validity invariant
    assert(aws_array_list_is_valid(&list));
}
