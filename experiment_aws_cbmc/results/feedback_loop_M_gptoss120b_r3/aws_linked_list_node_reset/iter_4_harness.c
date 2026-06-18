#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node node;

    /* Initialize the node with nondeterministic pointers */
    node.next = nondet_pointer();
    node.prev = nondet_pointer();

    /* Call the function under test */
    aws_linked_list_node_reset(&node);

    /* Post‑conditions: both links must be cleared */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
