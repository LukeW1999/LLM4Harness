#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_next_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node next_node;
    
    node.next = &next_node;
    node.prev = NULL;
    next_node.prev = &node;
    next_node.next = NULL;
    
    struct aws_linked_list_node *result = aws_linked_list_next(&node);
    
    assert(result == node.next);
}
