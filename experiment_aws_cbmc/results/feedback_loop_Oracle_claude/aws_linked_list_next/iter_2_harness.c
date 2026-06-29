#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_next_harness(void) {
    /* Allocate a node with non-deterministic content */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Allocate a next node that node->next will point to */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);

    /* Set up the node's next pointer */
    node->next = next_node;

    /* Save the original state for frame condition checks */
    struct aws_linked_list_node *original_next = node->next;
    struct aws_linked_list_node *original_prev = node->prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* Postcondition 1: Return value correctness
     * The returned pointer must equal node->next */
    assert(result == original_next);

    /* Postcondition 2: Return value is not NULL
     * (since we set it to a valid non-null pointer) */
    assert(result != NULL);

    /* Postcondition 3: The returned value points to the next_node we set up */
    assert(result == next_node);

    /* Frame condition: node->next was not modified by the function */
    assert(node->next == original_next);

    /* Frame condition: node->prev was not modified by the function */
    assert(node->prev == original_prev);
}
