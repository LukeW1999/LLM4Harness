#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;

    /* Assign nondeterministic values to the node's pointers */
    node.prev = (struct aws_linked_list_node *)__CPROVER_nondet_pointer();
    node.next = (struct aws_linked_list_node *)__CPROVER_nondet_pointer();

    /* Reset the node */
    aws_linked_list_node_reset(&node);

    /* After reset, both pointers must be NULL */
    __CPROVER_assert(node.prev == NULL, "node.prev is NULL after reset");
    __CPROVER_assert(node.next == NULL, "node.next is NULL after reset");
}
