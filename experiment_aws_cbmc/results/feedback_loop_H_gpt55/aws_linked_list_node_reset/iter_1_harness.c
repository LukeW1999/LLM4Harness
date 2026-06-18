#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness(void) {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&list.tail));

    bool was_empty = aws_linked_list_empty(&list);

    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;
    struct aws_linked_list_node *old_last_prev = old_last->prev;
    struct aws_linked_list_node *old_last_next = old_last->next;

    struct aws_linked_list_node node;
    node.next = NULL;
    node.prev = NULL;

    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(old_last_next == &list.tail);

    aws_linked_list_push_back(&list, &node);

    assert(list.tail.prev == &node);
    assert(node.prev == old_last);
    assert(node.next == &list.tail);
    assert(old_last->next == &node);

    if (was_empty) {
        assert(old_last == &old.head || old_last == &list.head);
        assert(list.head.next == &node);
        assert(node.prev == &list.head);
    } else {
        assert(list.head.next == old_first);
        assert(old_last->prev == old_last_prev);
    }

    assert(list.head.prev == old.head.prev);
    assert(list.head.prev == NULL);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.next == NULL);

    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_node_is_in_list(&node));

    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
