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

    /* Set up node->prev to point to prev_node (satisfies aws_linked_list_node_prev_is_valid) */
    node->prev = prev_node;

    /* node->next needs to be non-NULL for aws_linked_list_node_next_is_valid */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);
    node->next = next_node;

    /* prev_node->next needs to be non-NULL for aws_linked_list_node_next_is_valid(rval) */
    struct aws_linked_list_node *prev_next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_next_node != NULL);
    prev_node->next = prev_next_node;

    /* prev_node->prev needs to be non-NULL for aws_linked_list_node_prev_is_valid(rval) */
    struct aws_linked_list_node *prev_prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_prev_node != NULL);
    prev_node->prev = prev_prev_node;

    /* Also initialize next_node pointers to avoid any issues */
    next_node->next = node;
    next_node->prev = node;

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
