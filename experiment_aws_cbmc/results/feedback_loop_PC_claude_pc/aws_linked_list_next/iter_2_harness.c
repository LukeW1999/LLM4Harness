#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_next_harness() {
    /* 1. Declare and set up a linked list node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* node->next must be a valid (non-null) pointer for the function to be meaningful */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);

    /* next_node->prev must point back to node to satisfy aws_linked_list_node_next_is_valid */
    next_node->prev = node;

    /* next_node->next must be non-null to satisfy aws_linked_list_node_prev_is_valid(rval) */
    struct aws_linked_list_node *next_next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_next_node != NULL);
    next_node->next = next_next_node;

    /* Set up the node's next pointer */
    node->next = next_node;

    /* node->prev can be anything (not used by this function) */
    struct aws_linked_list_node *prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_node != NULL);
    node->prev = prev_node;

    /* Save old state BEFORE calling */
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node *old_prev = node->prev;

    /* 2. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 3. Assert postconditions */

    /* RETURN: The function returns node->next */
    assert(result == old_next);
    assert(result == next_node);

    /* FRAME: node itself must not be modified */
    assert(node->next == old_next);
    assert(node->prev == old_prev);

    /* INVARIANTS: result is the same pointer as node->next */
    assert(result == node->next);
}
