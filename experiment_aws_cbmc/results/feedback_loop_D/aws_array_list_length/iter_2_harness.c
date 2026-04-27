#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_length_harness() {
    struct aws_array_list list;
    struct aws_array_list old_list;
    size_t result;

    // Initialize list with arbitrary values
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    // Ensure the list is valid before the call
    __CPROVER_assume(aws_array_list_is_valid(&list));

    // Capture the old state of the list
    old_list = list;

    // Call the function under test
    result = aws_array_list_length(&list);

    // Step 1: Success path assertions
    if (result != AWS_OP_ERR) {
        assert(list.length == old_list.length);
    }

    // Step 2: Failure path assertions
    if (result == AWS_OP_ERR) {
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
    }

    // Step 3: Frame conditions
    assert(list.length == old_list.length);
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);
    assert(list.alloc == old_list.alloc);

    // Step 4: Validity invariants
    assert(aws_array_list_is_valid(&list));
}
