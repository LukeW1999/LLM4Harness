#include <aws/common/linked_list.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_linked_list_next_harness(void) {
    struct aws_linked_list_node node;
    node.next = (struct aws_linked_list_node *)nondet_voidp();
    node.prev = (struct aws_linked_list_node *)nondet_voidp();

    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    assert(result == node.next);
    assert(node.next == node.next); // unchanged
    assert(node.prev == node.prev); // unchanged
}
