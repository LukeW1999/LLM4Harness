#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node1;
    struct aws_linked_list_node node2;

    // Initialize list as empty (head and tail point to each other)
    list.head.next = &list.tail;
    list.head.prev = &list.tail;
    list.tail.next = &list.head;
    list.tail.prev = &list.head;

    // Ensure list is valid
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    // Push node1 into empty list
    struct aws_linked_list_node *old_last_before = list.tail.prev;
    aws_linked_list_push_back(&list, &node1);

    // Postconditions for push_back on empty list
    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == &node1);
    assert(node1.next == &list.tail);
    assert(node1.prev == old_last_before);
    assert(list.head.next == &node1);  // empty list case: head.next becomes node1

    // Now list has node1. Push node2 into non-empty list.
    struct aws_linked_list_node *old_last_before2 = list.tail.prev; // points to node1
    aws_linked_list_push_back(&list, &node2);

    // Postconditions for push_back on non-empty list
    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == &node2);
    assert(node2.next == &list.tail);
    assert(node2.prev == old_last_before2);
    assert(old_last_before2->next == &node2); // node1->next is node2
    assert(node1.prev == &list.head);
    assert(node1.next == &node2);
    assert(node2.prev == &node1);

    // Additional node validity checks
    assert(aws_linked_list_node_prev_is_valid(&node1));
    assert(aws_linked_list_node_next_is_valid(&node1));
    assert(aws_linked_list_node_prev_is_valid(&node2));
    assert(aws_linked_list_node_next_is_valid(&node2));
}
