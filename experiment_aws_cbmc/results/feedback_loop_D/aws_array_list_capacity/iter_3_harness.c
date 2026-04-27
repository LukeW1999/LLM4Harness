#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_capacity_harness() {
    struct aws_array_list list;
    size_t index = nondet_size_t();

    // Use default allocator
    struct aws_allocator *alloc = aws_default_allocator();
    assume(alloc != NULL);

    // Initialize the list with some arbitrary values
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    assume(item_size > 0);
    aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    // Ensure the list is valid before the call
    assert(aws_array_list_is_valid(&list));

    // Save old state
    struct aws_array_list old = list;

    // Assume index is within a reasonable range to avoid overflow
    __CPROVER_assume(index <= SIZE_MAX / item_size);

    size_t capacity;
    int result = aws_array_list_capacity(&list, &capacity);

    // Check frame conditions and postconditions based on the result
    if (result == AWS_OP_SUCCESS) {
        assert(capacity == list.current_size / list.item_size);
    } else {
        // In case of failure, the capacity should not be modified
        assert(capacity == 0);
    }

    // Check validity invariant
    assert(aws_array_list_is_valid(&list));

    // Clean up
    aws_array_list_clean_up(&list);
}
