#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_prev_harness(void) {
    /* Allocate a node non-deterministically */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *prev_node = malloc(sizeof(struct aws_linked_list_node));

    /* Preconditions */
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(prev_node != NULL);

    /* Set up the prev pointer */
    node->prev = prev_node;

    /* Ensure bidirectional link: prev_node->next == node */
    prev_node->next = node;

    /* prev_node->prev can be anything non-null (it's part of a list) */
    __CPROVER_assume(prev_node->prev != NULL);

    /* node->next can be anything non-null */
    __CPROVER_assume(node->next != NULL);

    /* Precondition: aws_linked_list_node_prev_is_valid(node) */
    /* This means node->prev->next == node, already set above */
    assert(aws_linked_list_node_prev_is_valid(node));

    /* Save values before call to check frame conditions */
    struct aws_linked_list_node *node_prev_before = node->prev;
    struct aws_linked_list_node *node_next_before = node->next;
    struct aws_linked_list_node *prev_node_next_before = prev_node->next;
    struct aws_linked_list_node *prev_node_prev_before = prev_node->prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* Postconditions (validity) */
    assert(result != NULL);
    assert(result == node_prev_before);
    assert(result == node->prev);

    /* Postconditions (frame): no memory modified */
    assert(node->prev == node_prev_before);
    assert(node->next == node_next_before);
    assert(prev_node->next == prev_node_next_before);
    assert(prev_node->prev == prev_node_prev_before);
}
