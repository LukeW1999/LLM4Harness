#include <aws/common/linked_list.h>
#include <stddef.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    
    node.next = malloc(sizeof(struct aws_linked_list_node));
    node.prev = malloc(sizeof(struct aws_linked_list_node));
    
    __CPROVER_assume(node.next != NULL);
    __CPROVER_assume(node.prev != NULL);
    
    aws_linked_list_node_reset(&node);
    
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
