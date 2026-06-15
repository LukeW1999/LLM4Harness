#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node node;

    /* Nondeterministically initialize the node's pointers */
    node.next = __CPROVER_nondet_pointer();
    node.prev = __CPROVER_nondet_pointer();

    /* Call the function under test */
    aws_linked_list_node_reset(&node);

    /* After reset, the node should point to itself */
    assert(node.next == &node);
    assert(node.prev == &node);
}
