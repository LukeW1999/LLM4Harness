#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness() {
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
    bool old_empty = aws_linked_list_empty(&list);

    aws_linked_list_push_back(&list, &node);

    assert(aws_linked_list_is_valid(&list));

    assert(list.tail.prev == &node);
    assert(node.prev == old_tail_prev);
    assert(node.next == &list.tail);
    assert(old_tail_prev->next == &node);

    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);

    if (old_empty) {
        assert(old_head_next == &list.tail);
        assert(old_tail_prev == &list.head);
        assert(list.head.next == &node);
        assert(node.prev == &list.head);
    } else {
        assert(list.head.next == old_head_next);
        assert(old_head_next->prev == &list.head);
    }

    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_node_is_in_list(&node));
}
