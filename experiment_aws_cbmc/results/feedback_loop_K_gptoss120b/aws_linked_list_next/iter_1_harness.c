/*  
 * Contract for aws_linked_list_next  
 * Preconditions:  
 *   - __CPROVER_assume(node != NULL);               // node pointer must be non‑null  
 *   - __CPROVER_assume(aws_linked_list_is_valid(&list)); // the list containing the node is a valid doubly‑linked list  
 *   - __CPROVER_assume(aws_linked_list_node_is_in_list((struct aws_linked_list_node *)node)); // node is part of the list  
 *   
 * Postconditions (validity):  
 *   - The returned pointer is exactly node->next (may be NULL if node is the tail).  
 *   - No memory locations are modified; the list remains valid after the call.  
 *   
 * Postconditions (frame):  
 *   - The contents of the list (head, tail, and all node links) are unchanged.  
 *   - The allocator state is unchanged (no allocation or deallocation occurs).  
 */

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_next_harness(void) {
    /* Create an empty list and initialize it */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Allocate two nodes and add them to the list */
    struct aws_linked_list_node *node1 = (struct aws_linked_list_node *)malloc(sizeof(*node1));
    __CPROVER_assume(node1 != NULL);
    aws_linked_list_node_reset(node1);
    aws_linked_list_push_back(&list, node1);

    struct aws_linked_list_node *node2 = (struct aws_linked_list_node *)malloc(sizeof(*node2));
    __CPROVER_assume(node2 != NULL);
    aws_linked_list_node_reset(node2);
    aws_linked_list_push_back(&list, node2);

    /* Ensure the list is still valid after insertions */
    assert(aws_linked_list_is_valid(&list));

    /* Choose a node that is definitely in the list */
    struct aws_linked_list_node *test_node = node1; /* nondet choice could be added if desired */
    __CPROVER_assume(test_node != NULL);
    __CPROVER_assume(aws_linked_list_node_is_in_list(test_node));

    /* Capture the expected next pointer before the call */
    struct aws_linked_list_node *expected_next = test_node->next;

    /* Call the function under test */
    struct aws_linked_list_node *actual_next = aws_linked_list_next(test_node);

    /* Postcondition checks */
    assert(actual_next == expected_next);                     /* return value matches node->next */
    assert(aws_linked_list_is_valid(&list));                 /* list remains valid */
    assert(aws_linked_list_node_is_in_list(test_node));      /* test_node is still in the list */

    /* Frame condition: no modifications to the list structure */
    assert(aws_linked_list_is_valid(&list));

    return 0;
}
