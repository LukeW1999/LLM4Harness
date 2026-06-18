#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node node;

    node.next = NULL;
    node.prev = NULL;

    assert(aws_linked_list_node_is_valid(&node));
    assert(!aws_linked_list_node_is_in_list(&node));

    aws_linked_list_node_reset(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(aws_linked_list_node_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(!aws_linked_list_node_is_in_list(&node));
}
