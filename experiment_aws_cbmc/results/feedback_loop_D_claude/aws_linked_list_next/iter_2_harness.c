#include <aws/common/linked_list.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_next_harness(void) {
    /* Allocate a node with nondeterministic content */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* node->next must be a valid (non-NULL) pointer since we return it */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);
    node->next = next_node;

    /* next_node->prev must point back to node for aws_linked_list_node_next_is_valid */
    next_node->prev = node;

    /* node->prev can be anything (not used by this function) */
    struct aws_linked_list_node *prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_node != NULL);
    node->prev = prev_node;

    /* Assume the precondition: aws_linked_list_node_next_is_valid(node) */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));

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
}
