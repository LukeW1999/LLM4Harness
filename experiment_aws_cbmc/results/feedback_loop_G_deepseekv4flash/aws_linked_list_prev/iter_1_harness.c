#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_prev_harness() {
    /* Create and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Ensure the list is not empty so we can pick an interior node */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Pick the first actual node (after head sentinel) */
    struct aws_linked_list_node *node = list.head.next;
    __CPROVER_assume(node != &list.tail);  // ensure it's a data node

    /* Save the old state of the node */
    struct aws_linked_list_node old_node = *node;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* Postcondition: returned value equals node->prev */
    assert(result == node->prev);

    /* The node's fields are unchanged */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* The list remains valid */
    assert(aws_linked_list_is_valid(&list));
}
