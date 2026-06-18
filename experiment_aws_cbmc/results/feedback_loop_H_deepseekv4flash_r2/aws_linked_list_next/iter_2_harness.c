#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_next_harness(void) {
    struct aws_linked_list_node prev, node, next;
    // Set up valid doubly linked list: prev <-> node <-> next
    prev.next = &node;
    prev.prev = NULL;
    node.prev = &prev;
    node.next = &next;
    next.prev = &node;
    next.next = NULL;

    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    assert(result == &next);
    assert(node.next == &next);
    assert(node.prev == &prev);
}
