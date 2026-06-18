#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node to_add;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    to_add.next = NULL;
    to_add.prev = NULL;

    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));
    __CPROVER_assume(!aws_linked_list_node_is_in_list(&to_add));

    struct aws_linked_list_node *after = &list.head;

    for (size_t i = 0; i < MAX_LINKED_LIST_ITEM_ALLOCATION; ++i) {
        if (nondet_bool() && after->next != &list.tail) {
            after = after->next;
        }
    }

    __CPROVER_assume(after != NULL);
    __CPROVER_assume(after != &list.tail);
    __CPROVER_assume(after != &to_add);
    __CPROVER_assume(aws_linked_list_node_next_is_valid(after));

    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_next_next = old_next->next;
    struct aws_linked_list_node *old_after_prev_next = NULL;

    __CPROVER_assume(old_next != NULL);
    __CPROVER_assume(old_next != &to_add);

    if (after != &list.head) {
        old_after_prev_next = old_after_prev->next;
    }

    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
    assert(!aws_linked_list_node_is_in_list(&to_add));

    aws_linked_list_insert_after(after, &to_add);

    assert(to_add.prev == after);
    assert(to_add.next == old_next);
    assert(after->next == &to_add);
    assert(old_next->prev == &to_add);

    assert(after->prev == old_after_prev);

    if (old_next != &list.tail) {
        assert(old_next->next == old_next_next);
    } else {
        assert(list.tail.next == old_next_next);
    }

    if (after != &list.head) {
        assert(old_after_prev->next == old_after_prev_next);
    }

    if (after == &list.head) {
        assert(list.head.next == &to_add);
    } else {
        assert(list.head.next == old_list.head.next);
    }

    assert(list.head.prev == old_list.head.prev);

    if (old_next == &list.tail) {
        assert(list.tail.prev == &to_add);
    } else {
        assert(list.tail.prev == old_list.tail.prev);
    }

    assert(list.tail.next == old_list.tail.next);

    assert(aws_linked_list_next(after) == &to_add);
    assert(aws_linked_list_prev(&to_add) == after);
    assert(aws_linked_list_next(&to_add) == old_next);
    assert(aws_linked_list_prev(old_next) == &to_add);

    assert(aws_linked_list_node_next_is_valid(after));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));

    assert(aws_linked_list_node_is_in_list(&to_add));
    assert(!aws_linked_list_empty(&list));

    assert(aws_linked_list_end(&list) == &list.tail);
    assert(aws_linked_list_rend(&list) == &list.head);

    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
