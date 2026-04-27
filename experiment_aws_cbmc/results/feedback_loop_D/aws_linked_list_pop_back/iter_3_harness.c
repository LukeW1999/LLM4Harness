#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list old_list;
    struct aws_linked_list_node node;
    struct aws_linked_list_node *popped_node;

    // Initialize list and node
    __CPROVER_assume(aws_linked_list_is_bounded(&list, MAX_LINKED_LIST_ITEM_ALLOCATION));
    old_list = list;
    node.next = NULL;
    node.prev = NULL;

    // Case 1: List is not empty
    __CPROVER_assume(!aws_linked_list_empty(&list));
    list.tail.prev = &node;
    node.next = &list.tail;
    list.head.next = &node;
    node.prev = &list.head;

    popped_node = aws_linked_list_pop_back(&list);

    // Success path assertions
    assert(popped_node == &node);
    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next == old_list.head.next);
    assert(list.tail.prev == (aws_linked_list_empty(&list) ? &list.head : old_list.tail.prev));
    assert(node.next == NULL);
    assert(node.prev == NULL);

    // Frame conditions
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);

    // Reset list for next case
    list = old_list;
    node.next = NULL;
    node.prev = NULL;

    // Case 2: List is empty
    __CPROVER_assume(aws_linked_list_empty(&list));
    __CPROVER_assume(list.head.next == &list.tail);
    __CPROVER_assume(list.tail.prev == &list.head);

    popped_node = aws_linked_list_pop_back(&list);

    // Failure path assertions
    assert(popped_node == NULL);
    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next == old_list.head.next);
    assert(list.tail.prev == old_list.tail.prev);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
}
