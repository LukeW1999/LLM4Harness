#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_next_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node next_node;
    
    /* Initialize node pointers */
    node.next = &next_node;
    node.prev = NULL;
    next_node.next = NULL;
    next_node.prev = &node;
    
    /* Call function */
    struct aws_linked_list_node *result = aws_linked_list_next(&node);
    
    /* Assert return value equals node.next */
    assert(result == &next_node);
    /* Assert node fields unchanged */
    assert(node.next == &next_node);
    assert(node.prev == NULL);
    /* Assert next_node fields unchanged */
    assert(next_node.next == NULL);
    assert(next_node.prev == &node);
}
