#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_node_reset_harness() {
    /* data structure */
    struct aws_linked_list_node node;

    /* perform operation under verification */
    aws_linked_list_node_reset(&node);

    /* assertions */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
