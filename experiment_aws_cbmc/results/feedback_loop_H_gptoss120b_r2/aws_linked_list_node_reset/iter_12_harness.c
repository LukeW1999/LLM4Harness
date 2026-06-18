#include <assert.h>
#include <aws/common/linked_list.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node node = {0};

    aws_linked_list_node_reset(&node);

    assert(node.next == &node);
    assert(node.prev == &node);
}
