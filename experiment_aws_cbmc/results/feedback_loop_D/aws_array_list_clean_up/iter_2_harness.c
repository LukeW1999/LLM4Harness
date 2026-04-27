#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clean_up_harness() {
    struct aws_array_list list;
    struct aws_array_list old_list = list;

    // Initialize list with non-deterministic values
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;
    list.current_size = nondet_size_t();
    list.length = nondet_size_t();
    list.item_size = nondet_size_t();
    list.data = (void *) nondet_ptr();

    // Ensure the list is bounded for safety
    ensure_array_list_has_allocated_data_member(&list);

    aws_array_list_clean_up(&list);

    // Assertions based on the analysis
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);

    // Check if the list is valid after cleanup
    assert(aws_array_list_is_valid(&list));
}
