#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    // Initialize list to be valid with up to MAX_LINKED_LIST_ITEM_ALLOCATION nodes
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    // Ensure node is not already part of any linked list
    node.prev = NULL;
    node.next = NULL;

    // Save pointer to previous last element
    struct aws_linked_list_node *old_last = list.tail.prev;

    // Call function under test
    aws_linked_list_push_back(&list, &node);

    // Postconditions
    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == &node);
    assert(node.next == &list.tail);
    assert(node.prev == old_last);

    // If the list was non-empty, the previous last element now points to node
    if (old_last != &list.head) {
        assert(old_last->next == &node);
    } else {
        assert(list.head.next == &node);
    }

    // Additional validity assertions for the inserted node
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
}
