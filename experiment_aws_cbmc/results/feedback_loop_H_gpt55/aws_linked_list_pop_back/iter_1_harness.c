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
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    aws_linked_list_node_reset(&node);
    assert(node.next == NULL);
    assert(node.prev == NULL);

    bool was_empty = aws_linked_list_empty(&list);
    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_last_prev = old_last->prev;

    assert(old_head_prev == NULL);
    assert(old_tail_next == NULL);
    assert(old_last->next == &list.tail);

    aws_linked_list_push_back(&list, &node);

    assert(list.tail.prev == &node);
    assert(node.prev == old_last);
    assert(node.next == &list.tail);
    assert(old_last->next == &node);

    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(old_last->prev == old_last_prev);

    if (was_empty) {
        assert(old_first == &list.tail);
        assert(old_last == &list.head);
        assert(list.head.next == &node);
    } else {
        assert(old_first != &list.tail);
        assert(old_last != &list.head);
        assert(list.head.next == old_first);
        assert(old_first->prev == &list.head);
    }

    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_node_is_in_list(&node));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
