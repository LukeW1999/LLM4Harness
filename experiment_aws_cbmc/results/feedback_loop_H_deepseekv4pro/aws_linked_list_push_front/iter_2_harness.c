#include <aws/common/linked_list.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node *node_ptr = &node;

    /* Precondition: node pointer is non-null */
    __CPROVER_assume(node_ptr != NULL);

    /* Initialize the node to a known state */
    node.next = NULL;
    node.prev = NULL;

    /* Call the function under test */
    aws_linked_list_node_reset(node_ptr);

    /* Postconditions from Doxygen and AWS_POSTCONDITION(AWS_IS_ZEROED(*node)) */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
