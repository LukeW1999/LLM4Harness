#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node node;

    /* nondet initialization of the node's pointers */
    node.next = __CPROVER_nondet_pointer();
    node.prev = __CPROVER_nondet_pointer();

    /* Call the function under verification */
    aws_linked_list_node_reset(&node);

    /* Post‑conditions: both links must be NULL */
    __CPROVER_assert(node.next == NULL, "node.next is NULL after reset");
    __CPROVER_assert(node.prev == NULL, "node.prev is NULL after reset");
}
