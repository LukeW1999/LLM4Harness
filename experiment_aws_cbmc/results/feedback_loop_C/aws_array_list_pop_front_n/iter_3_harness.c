#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_front_n_harness() {
    struct aws_array_list list;
    size_t n;

    // Initialize list with some arbitrary values
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(item_size > 0);
    aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);

    // Ensure list has allocated data member
    ensure_array_list_has_allocated_data_member(&list);

    // Save old state
    struct aws_array_list old_list = list;

    // Non-deterministic value for n
    n = nondet_size_t();
    __CPROVER_assume(n <= old_list.length);

    // Call the function under test
    aws_array_list_pop_front_n(&list, n);

    // Assertions based on the analysis
    if (n >= old_list.length) {
        assert(list.length == 0);
        assert(list.data == old_list.data); // Data pointer should not change if list is cleared
    } else if (n > 0) {
        size_t popping_bytes = old_list.item_size * n;
        size_t remaining_items = old_list.length - n;
        size_t remaining_bytes = remaining_items * old_list.item_size;
        assert(list.length == remaining_items);
        assert(list.data == (uint8_t *)old_list.data + popping_bytes);
    } else {
        assert(list.length == old_list.length);
        assert(list.data == old_list.data);
    }

    // Frame conditions
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);

    // Validity invariant
    assert(aws_array_list_is_valid(&list));

    // Clean up
    aws_array_list_clean_up(&list);
}
