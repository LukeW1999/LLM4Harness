#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_prev_harness() {
    /* Allocate the main node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Allocate the prev node */
    struct aws_linked_list_node *prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_node != NULL);

    /* Allocate the next node */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);

    /* Allocate prev_node's prev node */
    struct aws_linked_list_node *prev_prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_prev_node != NULL);

    /*
     * Set up a consistent doubly-linked list:
     * prev_prev_node <-> prev_node <-> node <-> next_node
     *
     * aws_linked_list_node_prev_is_valid(node): node->prev != NULL && node->prev->next == node
     * aws_linked_list_node_next_is_valid(rval): rval->next != NULL && rval->next->prev == rval
     *   where rval = prev_node
     */

    /* node->prev = prev_node, and prev_node->next = node (so prev_node->next->prev == prev_node... wait) */
    node->prev = prev_node;
    node->next = next_node;

    /* prev_node->next must equal node for aws_linked_list_node_prev_is_valid(node) */
    prev_node->next = node;
    /* prev_node->prev = prev_prev_node */
    prev_node->prev = prev_prev_node;

    /* next_node pointers - need consistency */
    next_node->prev = node;
    next_node->next = node; /* arbitrary, just non-NULL */

    /* prev_prev_node->next = prev_node for aws_linked_list_node_next_is_valid(prev_node) */
    /* aws_linked_list_node_next_is_valid(rval) where rval=prev_node:
       prev_node->next != NULL (yes, = node) AND prev_node->next->prev == prev_node
       i.e., node->prev == prev_node (yes, we set that) */
    prev_prev_node->next = prev_node;
    prev_prev_node->prev = prev_node; /* arbitrary non-NULL */

    /* Save old state */
    struct aws_linked_list_node *old_prev = node->prev;

    /* Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* Assert postconditions */
    assert(result == old_prev);
    assert(result == prev_node);
    assert(node->prev == old_prev);
    assert(result != NULL);
}
