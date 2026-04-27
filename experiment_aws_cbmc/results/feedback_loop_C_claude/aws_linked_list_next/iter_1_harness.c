// === STEP 1: SUCCESS PATH ===
// aws_linked_list_next always returns node->next (no failure path)
// The function simply reads node->next and returns it.
// Return value: equals node->next
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always succeeds
//
// === STEP 3: FRAME CONDITIONS ===
// node (struct aws_linked_list_node *):
//   - next: UNCHANGED (only read, not written)
//   - prev: UNCHANGED always
// The function does not modify any state.
//
// === STEP 4: VALIDITY INVARIANTS ===
// The node must be non-null and have a valid next pointer.
// aws_linked_list_node_next_is_valid(node): YES (should hold before call)
// After call: node->next and node->prev are unchanged.

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_next_harness(void) {
    /* Allocate a node and a next node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);

    /* Set up node->next to point to next_node */
    node->next = next_node;
    /* node->prev can be anything (nondet) */

    /* Save old state */
    struct aws_linked_list_node old_node = *node;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* === Assertions from Step 1: Return value === */
    assert(result == old_node.next);
    assert(result == next_node);

    /* === Assertions from Step 3: Frame conditions === */
    /* node->next is unchanged */
    assert(node->next == old_node.next);
    /* node->prev is unchanged */
    assert(node->prev == old_node.prev);
}
