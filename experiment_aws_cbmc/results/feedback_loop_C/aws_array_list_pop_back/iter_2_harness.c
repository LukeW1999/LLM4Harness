#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness() {
    struct aws_array_list list;
    struct aws_array_list old_list;
    size_t max_initial_item_allocation = nondet_size_t();
    size_t max_item_size = nondet_size_t();

    // Add assumption to ensure item_size is greater than 0
    __CPROVER_assume(max_item_size > 0);

    // Initialize list with some arbitrary values
    aws_array_list_init_dynamic(&list, aws_default_allocator(), max_initial_item_allocation, max_item_size);
    ensure_array_list_has_allocated_data_member(&list);

    // Save old state
    old_list = list;

    // Call the function under test
    int result = aws_array_list_pop_back(&list);

    // Success path assertions
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length - 1);
        // The last item should be zeroed out
        size_t last_item_offset = old_list.item_size * (old_list.length - 1);
        uint8_t zeroed_value = 0;
        assert_bytes_match((uint8_t *)old_list.data + last_item_offset, &zeroed_value, old_list.item_size);
    }

    // Failure path assertions
    if (result == AWS_OP_ERR) {
        assert(list.length == old_list.length);
        assert(list.data == old_list.data);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
    }

    // Validity invariant assertions
    assert(aws_array_list_is_valid(&list));
}
