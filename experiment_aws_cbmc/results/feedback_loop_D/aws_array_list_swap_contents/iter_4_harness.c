#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_swap_contents_harness() {
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    // Initialize list_a and list_b with some arbitrary values
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    // Initialize list_a
    __CPROVER_assume(aws_array_list_init(&list_a, allocator, 10, sizeof(int)) == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list_a));

    // Initialize list_b
    __CPROVER_assume(aws_array_list_init(&list_b, allocator, 10, sizeof(int)) == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    // Save old states
    struct aws_array_list old_list_a = list_a;
    struct aws_array_list old_list_b = list_b;

    // Call the function
    aws_array_list_swap_contents(&list_a, &list_b);

    // Assertions based on the analysis
    assert(list_a.alloc == old_list_b.alloc);
    assert(list_a.current_size == old_list_b.current_size);
    assert(list_a.length == old_list_b.length);
    assert(list_a.item_size == old_list_b.item_size);
    assert(list_a.data == old_list_b.data);

    assert(list_b.alloc == old_list_a.alloc);
    assert(list_b.current_size == old_list_a.current_size);
    assert(list_b.length == old_list_a.length);
    assert(list_b.item_size == old_list_a.item_size);
    assert(list_b.data == old_list_a.data);

    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));

    // Clean up
    aws_array_list_clean_up(&list_a);
    aws_array_list_clean_up(&list_b);
}
