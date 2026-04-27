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
    struct aws_linked_list_node prev_node;
    prev_node.next = &node;
    prev_node.prev = (struct aws_linked_list_node *)nondet_ptr();
    node.prev = &prev_node;
    node.next = &list.head;

    // Assume the node is valid
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&node));

    // Assume the list is valid and node is in the list
    __CPROVER_assume(list.head != NULL && list.tail != NULL);
    __CPROVER_assume(list.head->prev == &list.tail);
    __CPROVER_assume(list.tail->next == &list.head);
    __CPROVER_assume(node.prev != NULL && node.next != NULL);

    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    // Check frame conditions
    assert(node.next == old_node.next);
    assert(node.prev == old_node.prev);

    // Check postconditions
    if (result != NULL) {
        assert(result->next == &node);
    }

    // Check validity invariants
    assert(aws_linked_list_node_prev_is_valid(&node));
}
