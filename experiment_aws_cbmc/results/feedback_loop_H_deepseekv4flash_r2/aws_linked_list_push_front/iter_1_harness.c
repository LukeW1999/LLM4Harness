#include <aws/common/linked_list.h>
#include <assert.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    /* node is non-null */
    __CPROVER_assume(&node != NULL);

    /* call the function under test */
    aws_linked_list_node_reset(&node);

    /* postcondition: both pointers are NULL */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
