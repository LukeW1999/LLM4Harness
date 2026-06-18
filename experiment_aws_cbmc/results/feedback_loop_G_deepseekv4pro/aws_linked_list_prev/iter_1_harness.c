#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_prev_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node old_node = node;

    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    /* The function returns node->prev */
    assert(result == node.prev);

    /* The function does not modify the node */
    assert(node.next == old_node.next);
    assert(node.prev == old_node.prev);
}
