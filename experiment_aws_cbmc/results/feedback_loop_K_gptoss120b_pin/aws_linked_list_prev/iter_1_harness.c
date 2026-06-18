/*  
Preconditions:  
- __CPROVER_assume(node != NULL);  
- __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));  

Postconditions (validity):  
- assert(returned == node->prev);  

Postconditions (frame):  
- The linked list structure and all other memory locations remain unchanged after the call.  
*/

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_prev_harness(void) {
    /* Allocate and initialize a linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate a node and insert it into the list */
    struct aws_linked_list_node *node = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_node_reset(node);
    aws_linked_list_insert_after(&list.head, node);

    /* Ensure the node is in a valid state for prev */
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));

    /* Capture the expected prev pointer before the call */
    struct aws_linked_list_node *expected_prev = node->prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* Verify postconditions */
    assert(result == expected_prev);
    assert(aws_linked_list_is_valid(&list));

    /* Clean up */
    aws_linked_list_remove(node);
    free(node);
    return 0;
}
