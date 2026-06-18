#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;

    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_last_prev = old_last->prev;

    aws_linked_list_push_back(&list, &node);

    assert(list.tail.prev == &node);
    assert(node.next == &list.tail);
    assert(node.prev == old_last);
    assert(old_last->next == &node);

    if (old_first == &list.tail) {
        assert(list.head.next == &node);
    } else {
        assert(list.head.next == old_first);
    }

    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(old_last->prev == old_last_prev);

    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_prev_is_valid(&list.tail));
    assert(aws_linked_list_is_valid(&list));
}
