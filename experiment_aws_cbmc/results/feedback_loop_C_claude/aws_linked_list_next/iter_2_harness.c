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

    /* next_node needs a valid prev pointer for aws_linked_list_node_prev_is_valid(rval) */
    struct aws_linked_list_node *next_node_prev = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node_prev != NULL);
    next_node->prev = next_node_prev;

    /* node->prev can be anything nondet but must be valid for precondition */
    struct aws_linked_list_node *node_prev = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node_prev != NULL);
    node->prev = node_prev;

    /* Assume preconditions hold */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));

    /* Save old state */
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node *old_prev = node->prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* === Assertions from Step 1: Return value === */
    assert(result == old_next);
    assert(result == next_node);

    /* === Assertions from Step 3: Frame conditions === */
    /* node->next is unchanged */
    assert(node->next == old_next);
    /* node->prev is unchanged */
    assert(node->prev == old_prev);
}
