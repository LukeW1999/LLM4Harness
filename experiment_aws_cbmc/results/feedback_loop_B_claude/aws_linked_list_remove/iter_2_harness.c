#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

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

    /* Postcondition 2: node is reset — check that node is no longer connected to old neighbors */
    /* aws_linked_list_node_reset sets next=NULL, prev=NULL (not self-referential) */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* Postcondition 3: The list remains valid */
    assert(aws_linked_list_is_valid(&list));
}
