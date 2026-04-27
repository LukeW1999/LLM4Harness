#include <aws/common/linked_list.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_prev_harness(void) {
    /* Allocate a node non-deterministically */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    /* node must be non-NULL since we dereference it */
    __CPROVER_assume(node != NULL);

    /* Set up prev_node so that aws_linked_list_node_prev_is_valid holds */
    struct aws_linked_list_node *prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_node != NULL);

    /* prev_node->next must point back to node for aws_linked_list_node_prev_is_valid */
    prev_node->next = node;
    /* prev_node->prev can be anything non-NULL for validity */
    struct aws_linked_list_node *prev_prev = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_prev != NULL);
    prev_node->prev = prev_prev;

    node->prev = prev_node;

    /* node->next must be valid for aws_linked_list_node_next_is_valid */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);
    next_node->prev = node;
    next_node->next = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node->next != NULL);
    node->next = next_node;

    /* Assume preconditions hold */
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));

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
