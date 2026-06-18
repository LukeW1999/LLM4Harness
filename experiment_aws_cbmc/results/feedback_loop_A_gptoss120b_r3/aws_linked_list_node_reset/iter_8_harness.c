#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node node;

    /* The node may contain arbitrary (nondet) pointers before reset. */
    aws_linked_list_node_reset(&node);

    __CPROVER_assert(node.next == NULL, "node.next is NULL after reset");
    __CPROVER_assert(node.prev == NULL, "node.prev is NULL after reset");
}
