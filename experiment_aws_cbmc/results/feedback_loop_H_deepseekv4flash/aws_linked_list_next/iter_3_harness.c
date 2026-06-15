#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_next_harness() {
    struct aws_linked_list_node node;
    __CPROVER_havoc_object(&node2);
    struct aws_linked_list_node old_node = node;
    struct aws_linked_list_node *result = aws_linked_list_next(&node);
    assert(result == node.next);
    assert(node.next == old_node.next);
    assert(node.prev == old_node.prev);
}
