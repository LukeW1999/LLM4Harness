#include <aws/common/linked_list.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_remove_harness() {
    /* Create three nodes: prev, node, next */
    struct aws_linked_list_node prev_node;
    struct aws_linked_list_node node;
    struct aws_linked_list_node next_node;

    /* Link them together: prev_node <-> node <-> next_node */
    prev_node.next = &node;
    node.prev = &prev_node;
    node.next = &next_node;
    next_node.prev = &node;

    /* Also set outer pointers to something valid (not strictly needed for the test) */
    prev_node.prev = NULL;
    next_node.next = NULL;

    /* Call the function under test */
    aws_linked_list_remove(&node);

    /* Postcondition 1: prev_node->next now points to next_node */
    assert(prev_node.next == &next_node);

    /* Postcondition 2: next_node->prev now points to prev_node */
    assert(next_node.prev == &prev_node);

    /* Postcondition 3: node->next is reset (aws_linked_list_node_reset sets to NULL) */
    assert(node.next == NULL);

    /* Postcondition 4: node->prev is reset */
    assert(node.prev == NULL);
}
