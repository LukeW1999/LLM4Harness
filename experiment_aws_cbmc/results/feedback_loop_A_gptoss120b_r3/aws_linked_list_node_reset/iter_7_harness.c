#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node node;

    /* The node may contain arbitrary (nondet) pointers before reset.
       No additional assumptions are required. */

    aws_linked_list_node_reset(&node);

    __CPROVER_assert(node.next == &node, "node.next points to itself after reset");
    __CPROVER_assert(node.prev == &node, "node.prev points to itself after reset");
}
