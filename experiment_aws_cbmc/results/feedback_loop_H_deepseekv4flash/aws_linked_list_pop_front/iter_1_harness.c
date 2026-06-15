#include <aws/common/linked_list.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    /* Precondition: node != NULL (always true for &node) */
    /* Optionally assume non-null pointers to test reset */
    __CPROVER_assume(node.next != NULL && node.prev != NULL);
    aws_linked_list_node_reset(&node);
    /* Postcondition: next and prev are NULL */
    assert(node.next == NULL);
    assert(node.prev == NULL);
    /* The node is no longer in any list */
    assert(!aws_linked_list_node_is_in_list(&node));
}
