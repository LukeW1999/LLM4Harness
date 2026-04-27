#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_capacity_harness() {
    struct aws_array_list list;
    size_t index = nondet_size_t();
    struct aws_array_list old_list = list;

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

    int result = aws_array_list_capacity(&list, index);

    // Check frame conditions and postconditions based on the result
    if (result == AWS_OP_SUCCESS) {
        assert(list.current_size >= (index + 1) * list.item_size);
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
    } else {
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    }

    // Check validity invariant
    assert(aws_array_list_is_valid(&list));

    // Clean up
    aws_array_list_clean_up(&list);
}
