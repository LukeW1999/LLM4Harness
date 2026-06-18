#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_node_reset_harness() {
    /* Create a node on the stack */
    struct aws_linked_list_node node;

    /* The node's pointers are nondeterministic */
    /* Call the function under test */
    aws_linked_list_node_reset(&node);

    /* After reset, both pointers should be NULL */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
