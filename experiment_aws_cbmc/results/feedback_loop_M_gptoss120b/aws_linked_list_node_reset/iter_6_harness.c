#include <aws/common/linked_list.h>
#include <assert.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node node;

    /* Assume the node is not currently linked */
    __CPROVER_assume(node.next == NULL);
    __CPROVER_assume(node.prev == NULL);

    /* Call the function under test */
    aws_linked_list_node_reset(&node);

    /* After reset, the node should point to itself */
    assert(node.next == &node);
    assert(node.prev == &node);
}
