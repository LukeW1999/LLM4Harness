#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_node_reset_harness() {
    /* Non-deterministic node */
    struct aws_linked_list_node node;

    /* Ensure node pointer is valid (non-null) */
    __CPROVER_assume(node.next != NULL && node.prev != NULL);

    /* Save old values */
    struct aws_linked_list_node *old_next = node.next;
    struct aws_linked_list_node *old_prev = node.prev;

    /* Call the function */
    aws_linked_list_node_reset(&node);

    /* Postcondition: node points to itself */
    assert(node.next == &node);
    assert(node.prev == &node);

    /* Also ensure that the old pointers are not modified? Not required, but we can check that the node's pointers are set to itself */
    /* No other side effects */
}
