#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_prev_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node old_node = node;

    // Initialize node with arbitrary values
    node.next = (struct aws_linked_list_node *)nondet_ptr();
    node.prev = (struct aws_linked_list_node *)nondet_ptr();

    // Ensure the node is part of a valid linked list
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    // Ensure the node is part of the list
    node.next = &list.head;
    node.prev = &list.tail;

    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    // Check frame conditions
    assert(node.next == old_node.next);
    if (result != NULL) {
        assert(node.prev != old_node.prev);
    } else {
        assert(node.prev == old_node.prev);
    }

    // Check postconditions
    if (result != NULL) {
        assert(result->next == &node);
    }

    // Check validity invariants
    assert(aws_linked_list_node_prev_is_valid(&node));
}
