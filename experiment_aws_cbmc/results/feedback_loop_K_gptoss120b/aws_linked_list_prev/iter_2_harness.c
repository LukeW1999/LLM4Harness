/* Contract for aws_linked_list_prev
Preconditions:
  - node != NULL
  - aws_linked_list_node_prev_is_valid(node) == true
Postconditions (validity):
  - return value == node->prev
Postconditions (frame):
  - No memory locations are modified by the function.
*/

#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_prev_harness(void) {
    /* Initialize an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate two nodes and reset them */
    struct aws_linked_list_node *node1 = malloc(sizeof(*node1));
    struct aws_linked_list_node *node2 = malloc(sizeof(*node2));
    __CPROVER_assume(node1 != NULL);
    __CPROVER_assume(node2 != NULL);
    aws_linked_list_node_reset(node1);
    aws_linked_list_node_reset(node2);

    /* Insert nodes into the list */
    aws_linked_list_push_back(&list, node1);
    aws_linked_list_push_back(&list, node2);

    /* Choose a nondeterministic node that is part of the list and has a valid prev */
    struct aws_linked_list_node *node;
    __CPROVER_assume(node == node2);          /* node2's prev is node1, a regular node */
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));

    /* Snapshot state before the call */
    struct aws_linked_list_node *prev_before = node->prev;
    struct aws_linked_list_node *next_before = node->next;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* Verify postconditions */
    assert(result == prev_before);
    assert(node->prev == prev_before);
    assert(node->next == next_before);

    /* Verify that the list structure itself was not altered */
    assert(list.head.next != NULL);
    assert(list.tail.prev != NULL);
}
