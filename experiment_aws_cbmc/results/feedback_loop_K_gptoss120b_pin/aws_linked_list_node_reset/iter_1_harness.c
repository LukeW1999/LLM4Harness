/*=== Contract for aws_linked_list_node_reset ===
Preconditions:
  - node != NULL

Postconditions (validity):
  - node->next == NULL
  - node->prev == NULL
  - The memory occupied by *node is completely zeroed (AWS_IS_ZEROED)

Postconditions (frame):
  - No memory outside of the object pointed to by 'node' is modified.
===*/

#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness(void) {
    /* Allocate the node under test */
    struct aws_linked_list_node *node = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Nondeterministically initialize the node's fields */
    node->next = (struct aws_linked_list_node *)__CPROVER_make_nonzero_pointer();
    node->prev = (struct aws_linked_list_node *)__CPROVER_make_nonzero_pointer();

    /* Allocate a second node to serve as a frame check (should remain unchanged) */
    struct aws_linked_list_node *frame_node = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(frame_node != NULL);
    /* Save a copy of the frame node's original state */
    struct aws_linked_list_node frame_node_orig = *frame_node;

    /* Call the function under verification */
    aws_linked_list_node_reset(node);

    /* Verify postconditions */
    assert(node->next == NULL);
    assert(node->prev == NULL);
    /* Verify that the whole struct is zeroed */
    assert(AWS_IS_ZEROED(*node));

    /* Verify frame condition: memory outside of *node is untouched */
    assert(frame_node->next == frame_node_orig.next);
    assert(frame_node->prev == frame_node_orig.prev);

    /* Clean up */
    free(node);
    free(frame_node);
    return 0;
}
