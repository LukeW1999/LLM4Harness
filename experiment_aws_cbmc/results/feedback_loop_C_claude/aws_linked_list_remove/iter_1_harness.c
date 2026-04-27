// === STEP 1: SUCCESS PATH ===
// aws_linked_list_remove has no return value (void).
// After the call:
//   - node->prev->next = node->next (the node before now points forward past node)
//   - node->next->prev = node->prev (the node after now points backward past node)
//   - node->next = NULL (reset)
//   - node->prev = NULL (reset)
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void with no error return.
//
// === STEP 3: FRAME CONDITIONS ===
// node (struct aws_linked_list_node *):
//   - node->next: CHANGED to NULL (reset)
//   - node->prev: CHANGED to NULL (reset)
// node->prev (before call, call it prev_node):
//   - prev_node->next: CHANGED to old node->next
//   - prev_node->prev: UNCHANGED
// node->next (before call, call it next_node):
//   - next_node->prev: CHANGED to old node->prev
//   - next_node->next: UNCHANGED
//
// === STEP 4: VALIDITY INVARIANTS ===
// The list remains valid after removal (if it was valid before).
// The node's next and prev are NULL after removal.
// The prev_node and next_node are now linked to each other.

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_remove_harness(void) {
    /* Allocate a linked list with some elements */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    /* We need at least one element to remove */
    __CPROVER_assume(!aws_linked_list_empty(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* Pick a node to remove - use the first node for simplicity */
    struct aws_linked_list_node *node = aws_linked_list_begin(&list);
    /* node must not be head or tail */
    __CPROVER_assume(node != &list.head);
    __CPROVER_assume(node != &list.tail);
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node->next != NULL);
    __CPROVER_assume(node->prev != NULL);

    /* Save old state */
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_prev_prev = node->prev->prev;
    struct aws_linked_list_node *old_next_next = node->next->next;

    /* Call the function under test */
    aws_linked_list_remove(node);

    /* Assert frame conditions and postconditions */

    /* node->next and node->prev are reset to NULL */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* The previous node now points forward to old_next */
    assert(old_prev->next == old_next);

    /* The next node now points backward to old_prev */
    assert(old_next->prev == old_prev);

    /* prev_node->prev is unchanged */
    assert(old_prev->prev == old_prev_prev);

    /* next_node->next is unchanged */
    assert(old_next->next == old_next_next);

    /* The list remains valid */
    assert(aws_linked_list_is_valid(&list));
}
