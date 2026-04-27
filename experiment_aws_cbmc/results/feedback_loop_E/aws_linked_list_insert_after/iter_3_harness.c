#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_after_harness() {
    /* data structure */
    struct aws_linked_list list;
    struct aws_linked_list_node after;
    struct aws_linked_list_node to_add;

    /* allocation and initialization */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_node_is_allocated(&after));
    __CPROVER_assume(aws_linked_list_node_is_allocated(&to_add));
    __CPROVER_assume(aws_linked_list_node_is_in_list(&after));
    __CPROVER_assume(after.next != &list.tail); // Ensure 'after' is not the tail
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&after));

    /* Keep the old next node of 'after' */
    struct aws_linked_list_node *old_next = after.next;

    /* perform operation under verification */
    aws_linked_list_insert_after(&after, &to_add);

    /* assertions */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(after.next == &to_add);
    assert(to_add.prev == &after);
    assert(to_add.next == old_next);
    assert(old_next->prev == &to_add);
}
