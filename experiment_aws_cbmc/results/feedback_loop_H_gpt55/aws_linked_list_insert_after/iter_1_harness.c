#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    struct aws_linked_list_node to_add;
    aws_linked_list_node_reset(&to_add);
    __CPROVER_assume(!aws_linked_list_node_is_in_list(&to_add));

    struct aws_linked_list_node *after = &list.head;
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_LINKED_LIST_ITEM_ALLOCATION);

    for (size_t i = 0; i < MAX_LINKED_LIST_ITEM_ALLOCATION; ++i) {
        if (i < index && after->next != &list.tail) {
            after = after->next;
        }
    }

    __CPROVER_assume(after != NULL);
    __CPROVER_assume(&to_add != after);
    __CPROVER_assume(aws_linked_list_node_next_is_valid(after));
    __CPROVER_assume(!aws_linked_list_node_is_in_list(&to_add));

    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_next = after->next;
    struct aws_linked_list_node *old_next_next = old_next->next;

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    aws_linked_list_insert_after(after, &to_add);

    assert(to_add.prev == after);
    assert(to_add.next == old_next);
    assert(after->next == &to_add);
    assert(old_next->prev == &to_add);

    assert(after->prev == old_after_prev);
    assert(old_next->next == old_next_next);

    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);

    if (after == &list.head) {
        assert(list.head.next == &to_add);
        assert(aws_linked_list_begin(&list) == &to_add);
        assert(aws_linked_list_front(&list) == &to_add);
    } else {
        assert(list.head.next == old_head_next);
    }

    if (old_next == &list.tail) {
        assert(list.tail.prev == &to_add);
        assert(aws_linked_list_rbegin(&list) == &to_add);
        assert(aws_linked_list_back(&list) == &to_add);
    } else {
        assert(list.tail.prev == old_tail_prev);
    }

    assert(to_add.prev->next == &to_add);
    assert(to_add.next->prev == &to_add);

    assert(aws_linked_list_node_next_is_valid(after));
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_prev_is_valid(old_next));

    if (after != &list.head) {
        assert(aws_linked_list_node_prev_is_valid(after));
    }

    assert(aws_linked_list_node_is_in_list(&to_add));
    assert(!aws_linked_list_empty(&list));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
