// === STEP 1: SUCCESS PATH ===
// aws_linked_list_remove has no return value (void).
// After calling aws_linked_list_remove(node):
//   - node->prev->next: CHANGES to node->next (the node after the removed node)
//   - node->next->prev: CHANGES to node->prev (the node before the removed node)
//   - node->next: CHANGES to node itself (reset by aws_linked_list_node_reset)
//   - node->prev: CHANGES to node itself (reset by aws_linked_list_node_reset)
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void with no error handling.
//
// === STEP 3: FRAME CONDITIONS ===
// node (struct aws_linked_list_node *):
//   - node->next: CHANGED (reset to node itself)
//   - node->prev: CHANGED (reset to node itself)
// node->prev (the predecessor node):
//   - prev_node->next: CHANGED to old node->next
//   - prev_node->prev: UNCHANGED
// node->next (the successor node):
//   - next_node->next: UNCHANGED
//   - next_node->prev: CHANGED to old node->prev
//
// === STEP 4: VALIDITY INVARIANTS ===
// After removal, node->next == node and node->prev == node (reset state)
// The list remains valid: prev_node->next == next_node and next_node->prev == prev_node

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_remove_harness(void) {
    /* Allocate a list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    /* The list must be non-empty to remove a node */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Pick a node that is actually in the list (not head or tail sentinel) */
    /* We'll use the first real node: list.head.next */
    struct aws_linked_list_node *node = list.head.next;
    /* Ensure node is not the tail sentinel */
    __CPROVER_assume(node != &list.tail);
    /* Ensure node's prev and next are non-null (should be guaranteed by valid list) */
    __CPROVER_assume(node->prev != NULL);
    __CPROVER_assume(node->next != NULL);

    /* Save old state */
    struct aws_linked_list_node *old_node_next = node->next;
    struct aws_linked_list_node *old_node_prev = node->prev;
    struct aws_linked_list_node *old_prev_prev = node->prev->prev;
    struct aws_linked_list_node *old_next_next = node->next->next;

    /* Call the function under test */
    aws_linked_list_remove(node);

    /* === Assertions for node itself (reset state) === */
    /* aws_linked_list_node_reset sets next = node and prev = node */
    assert(node->next == node);
    assert(node->prev == node);

    /* === Assertions for predecessor node === */
    /* old_node_prev->next should now point to old_node_next */
    assert(old_node_prev->next == old_node_next);
    /* old_node_prev->prev should be unchanged */
    assert(old_node_prev->prev == old_prev_prev);

    /* === Assertions for successor node === */
    /* old_node_next->prev should now point to old_node_prev */
    assert(old_node_next->prev == old_node_prev);
    /* old_node_next->next should be unchanged */
    assert(old_node_next->next == old_next_next);

    /* === List validity invariant === */
    /* The list should still be valid after removal */
    assert(aws_linked_list_is_valid(&list));

    /* === Node is no longer in list (self-referential after reset) === */
    assert(!aws_linked_list_node_is_in_list(node));
}
