#include <aws/common/linked_list.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    /* Precondition: node != NULL (trivially true for stack variable) */
    aws_linked_list_node_reset(&node);
    /* Postcondition: node's next and prev pointers are NULL */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
