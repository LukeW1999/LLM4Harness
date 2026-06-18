#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_next_harness() {
    /* Allocate the main node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Allocate the next node */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);

    /* For aws_linked_list_node_next_is_valid(node): node->next must be non-NULL */
    node->next = next_node;

    /* node->prev must also be valid (non-NULL) for node to be valid */
    struct aws_linked_list_node *prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_node != NULL);
    node->prev = prev_node;

    /* For aws_linked_list_node_prev_is_valid(rval) where rval = next_node:
       next_node->prev must be non-NULL */
    struct aws_linked_list_node *next_prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_prev_node != NULL);
    next_node->prev = next_prev_node;

    /* next_node->next must also be non-NULL for next_node to be valid */
    struct aws_linked_list_node *next_next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_next_node != NULL);
    next_node->next = next_next_node;

    /* prev_node pointers - set to avoid any validity issues */
    prev_node->next = node;
    prev_node->prev = node; /* arbitrary but non-NULL */

    /* next_prev_node pointers */
    next_prev_node->next = next_node;
    next_prev_node->prev = next_node; /* arbitrary but non-NULL */

    /* next_next_node pointers */
    next_next_node->next = next_node;
    next_next_node->prev = next_node; /* arbitrary but non-NULL */

    /* Save old state */
    struct aws_linked_list_node *old_next = node->next;

    /* Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* Assert postconditions */
    assert(result == old_next);
    assert(result == node->next);
    assert(node->next == old_next);
}
