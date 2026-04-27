// === STEP 1: SUCCESS PATH ===
// aws_linked_list_prev always returns the prev pointer of the node.
// There is no failure path - it simply returns node->prev.
// Return value: node->prev (the previous node pointer)
//
// === STEP 2: FAILURE PATH ===
// No failure path exists for this function.
//
// === STEP 3: FRAME CONDITIONS ===
// node (struct aws_linked_list_node *):
//   - next: UNCHANGED always (function only reads prev)
//   - prev: UNCHANGED always (function only reads prev, doesn't modify it)
// The function is read-only - it does not modify any state.
//
// === STEP 4: VALIDITY INVARIANTS ===
// - The node must have a valid prev pointer (non-null for a node in a list)
// - aws_linked_list_node_prev_is_valid(node): should hold if node is in a valid list
// - The returned value equals node->prev

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_prev_harness(void) {
    /* Allocate a linked list with bounded length */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    
    /* Assume the list is valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* We need a node that is in the list - use a non-deterministic node
     * that could be any node in the list. For simplicity, we can use
     * the tail sentinel node, which always has a valid prev pointer
     * (pointing to either head or a real node). */
    
    /* Pick the tail node as our test node - it always has a valid prev */
    struct aws_linked_list_node *node = &list.tail;
    
    /* Save old state */
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node *old_prev = node->prev;
    
    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);
    
    /* Verify return value equals node->prev */
    assert(result == old_prev);
    
    /* Verify frame conditions - node is unchanged */
    assert(node->next == old_next);
    assert(node->prev == old_prev);
    
    /* Verify the list is still valid after the call */
    assert(aws_linked_list_is_valid(&list));
}
