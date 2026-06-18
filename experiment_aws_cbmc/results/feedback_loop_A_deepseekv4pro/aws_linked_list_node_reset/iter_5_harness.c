#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;

    __CPROVER_assume(aws_linked_list_node_is_valid(&node));

    aws_linked_list_node_reset(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
}
