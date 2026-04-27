#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_back_harness() {
    struct aws_linked_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(aws_linked_list_is_bounded(&list, MAX_LINKED_LIST_ITEM_ALLOCATION));
    aws_linked_list_init(&list, allocator);

    // Initialize the list with at least one element for success case
    struct aws_linked_list_node node;
    aws_linked_list_node_init(&node);
    aws_linked_list_push_back(&list, &node);

    // Save old state
    struct aws_linked_list old_list = list;

    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    // Assert frame conditions
    assert(list.head.next == &node);
    assert(list.head.prev == &list.head);
    assert(list.tail.prev == &node);
    assert(list.tail.next == &list.tail);
    assert(result == &node);

    // Assert validity invariants
    assert(aws_linked_list_is_valid(&list));

    // Test failure case by emptying the list
    aws_linked_list_pop_back(&list);

    // Save old state for failure case
    old_list = list;

    result = aws_linked_list_back(&list);

    // Assert frame conditions for failure case
    assert(list.head.next == &list.tail);
    assert(list.head.prev == &list.head);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == &list.tail);
    assert(result == NULL);

    // Assert validity invariants for failure case
    assert(aws_linked_list_is_valid(&list));
}
