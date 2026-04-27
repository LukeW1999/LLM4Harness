#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_swap_contents_harness() {
    /* data structures */
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    /* assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list_a, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_bounded(&list_b, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list_a);
    ensure_array_list_has_allocated_data_member(&list_b);
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));
    __CPROVER_assume(list_a.item_size == list_b.item_size);
    __CPROVER_assume(&list_a != &list_b);

    /* save old states */
    struct aws_array_list old_list_a = list_a;
    struct aws_array_list old_list_b = list_b;

    /* perform operation under verification */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* assertions */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));

    /* Check that the contents have been swapped */
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
}
