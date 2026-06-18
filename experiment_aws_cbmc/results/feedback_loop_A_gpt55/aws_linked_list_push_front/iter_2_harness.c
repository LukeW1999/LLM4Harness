#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_front_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_node_reset(&node);

    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&list.head));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&list.tail));
    __CPROVER_assume(!aws_linked_list_node_is_in_list(&node));
    __CPROVER_assume(list.head.next != &node);
    __CPROVER_assume(list.tail.prev != &node);

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    int old_empty = aws_linked_list_empty(&list);

    aws_linked_list_push_front(&list, &node);

    __CPROVER_assert(aws_linked_list_is_valid(&list), "list is valid after push_front");

    __CPROVER_assert(list.head.next == &node, "new node is first");
    __CPROVER_assert(node.prev == &list.head, "new node prev is head");
    __CPROVER_assert(node.next == old_head_next, "new node next is old first");
    __CPROVER_assert(old_head_next->prev == &node, "old first prev is new node");

    __CPROVER_assert(list.head.prev == old_head_prev, "head prev unchanged");
    __CPROVER_assert(list.tail.next == old_tail_next, "tail next unchanged");

    if (old_empty) {
        __CPROVER_assert(old_head_next == &list.tail, "empty list old head next was tail");
        __CPROVER_assert(old_tail_prev == &list.head, "empty list old tail prev was head");
        __CPROVER_assert(list.tail.prev == &node, "tail prev is new node");
        __CPROVER_assert(node.next == &list.tail, "new node next is tail");
    } else {
        __CPROVER_assert(list.tail.prev == old_tail_prev, "tail prev unchanged for non-empty list");
        __CPROVER_assert(old_tail_prev->next == &list.tail, "old last still points to tail");
    }

    __CPROVER_assert(aws_linked_list_node_prev_is_valid(&node), "new node prev is valid");
    __CPROVER_assert(aws_linked_list_node_next_is_valid(&node), "new node next is valid");
    __CPROVER_assert(aws_linked_list_node_is_in_list(&node), "new node is in list");
}
