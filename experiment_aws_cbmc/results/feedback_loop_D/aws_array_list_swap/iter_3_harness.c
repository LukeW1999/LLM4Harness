#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_swap_harness() {
    struct aws_array_list list;
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    // Use default allocator
    struct aws_allocator *allocator = aws_default_allocator();

    // Ensure the list is properly allocated and initialized
    ensure_array_list_has_allocated_data_member(&list, allocator);
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(a < list.length && b < list.length);

    // Save old state
    struct aws_array_list old_list = list;

    // Call the function under test
    int result = aws_array_list_swap(&list, a, b);

    // Assertions for success path
    if (result == AWS_OP_SUCCESS) {
        if (a != b) {
            // Check that the elements at indices a and b have been swapped
            assert(memcmp((char *)list.data + a * list.item_size, (char *)old_list.data + b * old_list.item_size, list.item_size) == 0);
            assert(memcmp((char *)list.data + b * list.item_size, (char *)old_list.data + a * old_list.item_size, list.item_size) == 0);
        }
    } else {
        // Assertions for failure path
        assert(memcmp(list.data, old_list.data, list.length * list.item_size) == 0);
    }

    // Assertions for frame conditions
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    // Assertions for validity invariants
    assert(aws_array_list_is_valid(&list));
}
