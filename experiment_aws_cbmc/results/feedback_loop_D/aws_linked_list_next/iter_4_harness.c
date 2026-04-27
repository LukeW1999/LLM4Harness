#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_next_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node old_node = node;

    // Initialize node with non-deterministic values
    node.next = (struct aws_linked_list_node *)nondet_ptr();
    node.prev = (struct aws_linked_list_node *)nondet_ptr();

    // Ensure the node is part of a valid linked list
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_node_is_valid(&node));
    __CPROVER_assume(node.next != NULL);
    __CPROVER_assume(node.prev != NULL);

    // Add assumptions to ensure the next and prev nodes are valid
    __CPROVER_assume(aws_linked_list_node_is_valid(node.next));
    __CPROVER_assume(aws_linked_list_node_is_valid(node.prev));

    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    // Success path assertions
    if (result != NULL) {
        assert(node.next == result);
        assert(node.prev == old_node.prev);
        assert(aws_linked_list_node_next_is_valid(&node));
    }
    // Failure path assertions
    else {
        assert(node.next == old_node.next);
        assert(node.prev == old_node.prev);
    }
}
