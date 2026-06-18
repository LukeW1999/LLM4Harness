#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_next_harness() {
    /* Allocate nodes */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);

    struct aws_linked_list_node *next_prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_prev_node != NULL);

    /* Set up node->next to satisfy aws_linked_list_node_next_is_valid(node):
       node->next must be non-NULL and node->next->prev must equal node */
    node->next = next_node;
    next_node->prev = node;

    /* Set up next_node->next to satisfy aws_linked_list_node_prev_is_valid(rval):
       rval->prev must be non-NULL and rval->prev->next must equal rval */
    next_node->next = next_prev_node;
    next_prev_node->next = next_node; /* not strictly needed but keeps structure consistent */

    /* Precondition: aws_linked_list_node_next_is_valid(node) must hold */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(next_node));

    /* Save old state */
    struct aws_linked_list_node *old_next = node->next;

    /* Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* Assert postconditions */
    assert(result == old_next);
    assert(result == next_node);
    assert(node->next == old_next);
}
