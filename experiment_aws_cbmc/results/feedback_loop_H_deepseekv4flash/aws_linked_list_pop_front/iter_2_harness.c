#include <aws/common/linked_list.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    
    /* Initialize node pointers to non-NULL values to ensure reset works */
    node.next = &node;
    node.prev = &node;
    
    /* Call the function under test */
    aws_linked_list_node_reset(&node);
    
    /* Postcondition: next and prev are NULL */
    assert(node.next == NULL);
    assert(node.prev == NULL);
    
    /* The node is no longer in any list */
    assert(!aws_linked_list_node_is_in_list(&node));
}
