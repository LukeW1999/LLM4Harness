#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_next_harness(void) {
    /* Allocate a node under test */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Allocate the next node */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);

    /* Set up the forward link */
    node->next = next_node;

    /* Precondition: next_is_valid means next->prev == node */
    next_node->prev = node;

    /* node->prev can be anything (non-deterministic), not relevant here */
    /* next_node->next can be anything (non-deterministic) */

    /* Save state before call to verify frame conditions */
    struct aws_linked_list_node *node_next_before = node->next;
    struct aws_linked_list_node *node_prev_before = node->prev;
    struct aws_linked_list_node *next_node_next_before = next_node->next;
    struct aws_linked_list_node *next_node_prev_before = next_node->prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* Postcondition: return value equals node->next */
    assert(result == node_next_before);

    /* Postcondition: return value is not NULL */
    assert(result != NULL);

    /* Postcondition: result is exactly next_node */
    assert(result == next_node);

    /* Frame conditions: node is not modified */
    assert(node->next == node_next_before);
    assert(node->prev == node_prev_before);

    /* Frame conditions: next_node is not modified */
    assert(next_node->next == next_node_next_before);
    assert(next_node->prev == next_node_prev_before);
}

void aws_linked_list_next_harness(void) {
    aws_linked_list_next_harness();
    return 0;
}
