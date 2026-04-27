#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_remove_harness() {
    /* data structure */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume the list is not empty and the node is in the list */
    __CPROVER_assume(!aws_linked_list_empty(&list));
    __CPROVER_assume(aws_linked_list_node_is_in_list(&node));

    /* Assume the node is not the head or tail */
    __CPROVER_assume(node.next != &list.tail);
    __CPROVER_assume(node.prev != &list.head);

    /* Ensure the node's prev and next pointers are valid */
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&node));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node));

    /* Save the old state of the list and the node */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = node;

    /* Save the previous and next nodes */
    struct aws_linked_list_node *prev_node = node.prev;
    struct aws_linked_list_node *next_node = node.next;

    /* perform operation under verification */
    aws_linked_list_remove(&node);

    /* assertions */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_next_is_valid(prev_node));
    assert(aws_linked_list_node_prev_is_valid(next_node));
    assert(prev_node->next == next_node);
    assert(next_node->prev == prev_node);
    assert(node.next == NULL);
    assert(node.prev == NULL);

    /* Check that other nodes in the list are unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.tail.prev == old_list.tail.prev);
}
