#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clear_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;
    struct aws_array_list old_list;
    size_t max_item_alloc = nondet_size_t();
    size_t max_item_size = nondet_size_t();

    make_and_copy_array_list(&list, &old_list, max_item_alloc, max_item_size, allocator);
    __CPROVER_assume(aws_array_list_is_bounded(&list, max_item_alloc, max_item_size));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    aws_array_list_clear(&list);

    // Frame conditions
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    // Validity invariant
    assert(aws_array_list_is_valid(&list));

    // Success path
    assert(list.length == 0);
}
