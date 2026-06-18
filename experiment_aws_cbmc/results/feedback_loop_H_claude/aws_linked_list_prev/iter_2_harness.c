#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_prev_harness() {
    /* Allocate nodes */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    struct aws_linked_list_node *prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_node != NULL);

    struct aws_linked_list_node *prev_prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_prev_node != NULL);

    /* Set up a valid doubly-linked list segment:
     * prev_prev_node <-> prev_node <-> node
     * For aws_linked_list_node_prev_is_valid(node):
     *   node->prev = prev_node, and prev_node->next = node
     * For aws_linked_list_node_next_is_valid(prev_node) (the return value):
     *   prev_node->next = node (already set), and node->prev = prev_node (already set)
     * For aws_linked_list_node_prev_is_valid(prev_node):
     *   prev_node->prev = prev_prev_node, and prev_prev_node->next = prev_node
     */

    /* Set up prev_node */
    prev_node->prev = prev_prev_node;
    prev_node->next = node;

    /* Set up node */
    node->prev = prev_node;
    /* node->next can be non-deterministic but must be non-NULL for validity */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);
    node->next = next_node;
    next_node->prev = node;

    /* Set up prev_prev_node */
    prev_prev_node->next = prev_node;
    /* prev_prev_node->prev can be non-deterministic but non-NULL */
    struct aws_linked_list_node *prev_prev_prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_prev_prev_node != NULL);
    prev_prev_node->prev = prev_prev_prev_node;
    prev_prev_prev_node->next = prev_prev_node;

    /* Save old state */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* Assert postconditions */
    assert(result == old_prev);
    assert(result == prev_node);

    /* Assert node is not modified */
    assert(node->prev == old_prev);
    assert(node->next == old_next);
}
