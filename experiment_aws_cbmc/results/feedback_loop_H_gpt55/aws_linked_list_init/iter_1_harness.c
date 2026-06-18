#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    bool was_empty = aws_linked_list_empty(&list);
    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    __CPROVER_assume(&node != &list.head);
    __CPROVER_assume(&node != &list.tail);
    __CPROVER_assume(node.next != &node);
    __CPROVER_assume(node.prev != &node);

    aws_linked_list_push_back(&list, &node);

    assert(list.tail.prev == &node);
    assert(node.prev == old_last);
    assert(node.next == &list.tail);
    assert(old_last->next == &node);

    if (was_empty) {
        assert(list.head.next == &node);
        assert(node.prev == &list.head);
    } else {
        assert(list.head.next == old_first);
    }

    assert(list.head.prev == old_head_prev);
    assert(list.head.prev == NULL);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.next == NULL);

    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_is_valid(&list));
}
