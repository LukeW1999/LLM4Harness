// === STEP 1: SUCCESS PATH ===
// aws_linked_list_next always returns node->next (no failure path)
// The function simply reads node->next and returns it.
// Return value: equals node->next
//
// === STEP 2: FAILURE PATH ===
// No failure path exists. The function always succeeds.
//
// === STEP 3: FRAME CONDITIONS ===
// param: node (const struct aws_linked_list_node *)
//   - node->next: UNCHANGED (only read, not written)
//   - node->prev: UNCHANGED always
// The function does not modify any state.
//
// === STEP 4: VALIDITY INVARIANTS ===
// - node must be non-NULL (dereferenced)
// - node->next must be a valid pointer (returned)
// - aws_linked_list_node_next_is_valid(node): YES (node->next must be valid)

#include <aws/common/linked_list.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_next_harness(void) {
    /* Allocate a node with nondeterministic content */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* node->next must be a valid (non-NULL) pointer since we return it */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);
    node->next = next_node;

    /* node->prev can be anything (not used by this function) */
    /* Save old state */
    struct aws_linked_list_node old_node = *node;

    /* Call the function */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* === Assertions from Step 1: return value === */
    assert(result == old_node.next);

    /* === Assertions from Step 3: frame conditions === */
    /* node->next is unchanged */
    assert(node->next == old_node.next);
    /* node->prev is unchanged */
    assert(node->prev == old_node.prev);

    /* === Assertions from Step 4: validity invariants === */
    /* result must be non-NULL (we assumed next_node != NULL) */
    assert(result != NULL);
    /* node->next is valid (non-NULL) */
    assert(aws_linked_list_node_next_is_valid(node));
}
