#include <aws/common/linked_list.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    /* node is uninitialized; function will zero it */
    aws_linked_list_node_reset(&node);
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
