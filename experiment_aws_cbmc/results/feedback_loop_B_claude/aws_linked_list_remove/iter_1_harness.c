#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

/**
 * aws_linked_list_remove implementation:
 *   node->prev->next = node->next;
 *   node->next->prev = node->prev;
 *   aws_linked_list_node_reset(node);  // sets node->next = node->prev = NULL (or node itself)
 *
 * Preconditions:
 *   - node is non-NULL
 *   - node->prev is non-NULL and node->prev->next == node
 *   - node->next is non-NULL and node->next->prev == node
 *   - The list containing node is valid
 *
 * Postconditions:
 *   - node->prev->next == node->next (the neighbors are linked to each other)
 *   - node->next->prev == node->prev
 *   - node is reset (next and prev point to node itself, per aws_linked_list_node_reset)
 *   - The list remains valid
 */

void aws_linked_list_remove_harness(void) {
    /* Set up a valid linked list with at least one node */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* The list must be non-empty for remove to be valid */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Pick the node to remove — use the front node for simplicity */
    struct aws_linked_list_node *node = aws_linked_list_begin(&list);

    /* node must be a real node (not the sentinel tail) */
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node != &list.tail);
    __CPROVER_assume(node->prev != NULL);
    __CPROVER_assume(node->next != NULL);

    /* Save the neighbors before removal */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* Call the function under test */
    aws_linked_list_remove(node);

    /* Postcondition 1: The neighbors are now linked to each other */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* Postcondition 2: node is reset — aws_linked_list_node_reset sets next=node, prev=node */
    assert(node->next == node);
    assert(node->prev == node);

    /* Postcondition 3: The list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 4: node is no longer in the list (node_is_in_list checks next != node) */
    /* After reset, node->next == node, so aws_linked_list_node_is_in_list returns false */
    assert(!aws_linked_list_node_is_in_list(node));
}
