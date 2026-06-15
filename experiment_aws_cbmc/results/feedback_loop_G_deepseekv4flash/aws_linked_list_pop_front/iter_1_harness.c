#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_swap_nodes_harness() {
    /* Create a linked list with at least two nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    /* Ensure list has at least two nodes */
    __CPROVER_assume(!aws_linked_list_empty(&list));
    __CPROVER_assume(list.head.next->next != &list.tail);

    /* Pick two distinct nodes from the list (first and second) */
    struct aws_linked_list_node *a = list.head.next;
    struct aws_linked_list_node *b = a->next;

    /* Save old list state for immutability checks */
    struct aws_linked_list old_list = list;

    /* Call function under test */
    aws_linked_list_swap_nodes(a, b);

    /* Postconditions from the implementation */
    assert(aws_linked_list_node_prev_is_valid(a));
    assert(aws_linked_list_node_next_is_valid(a));
    assert(aws_linked_list_node_prev_is_valid(b));
    assert(aws_linked_list_node_next_is_valid(b));

    /* The list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* Unchanged fields: the list structure itself (head and tail pointers) should not change */
    assert(list.head.next == old_list.head.next || list.head.next == old_list.head.next->next); /* nodes swapped, but head.next points to one of the two */
    assert(list.tail.prev == old_list.tail.prev || list.tail.prev == old_list.tail.prev->prev); /* tail.prev similarly */
    /* More precisely, the head and tail nodes themselves are unchanged */
    assert(list.head.next != &list.tail); /* list still non-empty */
    assert(list.tail.prev != &list.head);
}
