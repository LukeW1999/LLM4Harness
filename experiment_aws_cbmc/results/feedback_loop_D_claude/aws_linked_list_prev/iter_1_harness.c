// === STEP 1: SUCCESS PATH ===
// aws_linked_list_prev always returns node->prev (no failure path)
// The function simply reads node->prev and returns it.
// No state is modified.
//
// === STEP 2: FAILURE PATH ===
// No failure path exists. The function always returns node->prev.
//
// === STEP 3: FRAME CONDITIONS ===
// node (struct aws_linked_list_node *):
//   - next: UNCHANGED always
//   - prev: UNCHANGED always
// The function does not modify any state.
//
// === STEP 4: VALIDITY INVARIANTS ===
// The node must be non-NULL (we dereference it).
// The returned value equals node->prev.
// aws_linked_list_node_prev_is_valid(node): should hold if node->prev is valid

#include <aws/common/linked_list.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_prev_harness(void) {
    /* Allocate a node non-deterministically */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    /* node must be non-NULL since we dereference it */
    __CPROVER_assume(node != NULL);

    /* Set up non-deterministic prev pointer */
    struct aws_linked_list_node *prev_node = malloc(sizeof(struct aws_linked_list_node));
    node->prev = prev_node;

    /* Save old state */
    struct aws_linked_list_node old_node = *node;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* === STEP 1 assertions: return value equals node->prev === */
    assert(result == old_node.prev);

    /* === STEP 3 assertions: frame conditions - node is unchanged === */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);
}
