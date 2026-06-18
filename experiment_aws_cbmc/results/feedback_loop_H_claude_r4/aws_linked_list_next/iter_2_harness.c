#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_next_harness() {
    /* 1. Declare and set up linked list nodes */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);

    struct aws_linked_list_node *next_next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_next_node != NULL);

    struct aws_linked_list_node *prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_node != NULL);

    /* Set up node: node->next = next_node, node->prev = prev_node */
    node->next = next_node;
    node->prev = prev_node;

    /* For aws_linked_list_node_next_is_valid(node):
       next_node->prev must equal node */
    next_node->prev = node;

    /* For aws_linked_list_node_prev_is_valid(rval) where rval = next_node:
       next_node->prev must be valid, i.e., next_node->prev->next == next_node */
    /* next_node->prev = node, so node->next must == next_node (already set) */

    /* next_node->next must also be valid for any checks on next_node */
    next_node->next = next_next_node;
    next_next_node->prev = next_node;
    next_next_node->next = NULL; /* not needed for this check */

    /* prev_node->next must equal node for prev validity */
    prev_node->next = node;
    prev_node->prev = NULL; /* not needed */

    /* 2. Save old state */
    struct aws_linked_list_node *old_next = node->next;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 4. Assert postconditions */
    assert(result == old_next);
    assert(result == next_node);
    assert(node->next == old_next);
}
