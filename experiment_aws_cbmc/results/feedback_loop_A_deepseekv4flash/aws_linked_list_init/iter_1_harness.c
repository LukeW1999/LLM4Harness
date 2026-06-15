#include <aws/common/linked_list.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;

    /* Precondition: node is non-null (satisfied by local variable) */
    __CPROVER_assume(&node != NULL);

    aws_linked_list_node_reset(&node);

    /* Postcondition: both pointers set to NULL */
    assert(node.next == NULL);
    assert(node.prev == NULL);

    /* No other fields to check; struct has only next and prev */
}
