#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_prev_harness() {
    /* Allocate and initialize a valid linked list node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    assert(node != NULL);

    /* Initialize the node to a valid state: self-looping */
    node->next = node;
    node->prev = node;

    /* Save the original state for immutability checks */
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node *old_prev = node->prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* Postcondition 1: return value equals node->prev */
    assert(result == node->prev);

    /* Postcondition 2: the node itself is not modified (const function) */
    assert(node->next == old_next);
    assert(node->prev == old_prev);

    free(node);
}
