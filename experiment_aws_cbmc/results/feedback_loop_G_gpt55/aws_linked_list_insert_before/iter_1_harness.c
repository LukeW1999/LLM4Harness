#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_linked_list_insert_before_harness(void) {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    struct aws_linked_list_node *before = list.head.next;
    for (size_t i = 0; i < MAX_LINKED_LIST_ITEM_ALLOCATION; ++i) {
        if (nondet_bool() && before != &list.tail) {
            before = before->next;
        }
    }

    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before != &list.head);
    __CPROVER_assume(before->prev != NULL);
    __CPROVER_assume(before->prev->next == before);

    struct aws_linked_list_node to_add;
    to_add.next = NULL;
    to_add.prev = NULL;

    __CPROVER_assume(before != &to_add);
    __CPROVER_assume(before->prev != &to_add);
    __CPROVER_assume(!aws_linked_list_node_is_in_list(&to_add));

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;
    struct aws_linked_list_node *old_before_prev_prev = old_before_prev->prev;

    struct aws_linked_list_node *old_to_add_next = to_add.next;
    struct aws_linked_list_node *old_to_add_prev = to_add.prev;

    aws_linked_list_insert_before(before, &to_add);

    assert(to_add.next == before);
    assert(to_add.prev == old_before_prev);
    assert(to_add.next != old_to_add_next);
    assert(to_add.prev != old_to_add_prev);

    assert(before->prev == &to_add);
    assert(before->next == old_before_next);

    assert(old_before_prev->next == &to_add);
    assert(old_before_prev->prev == old_before_prev_prev);

    assert(to_add.next->prev == &to_add);
    assert(to_add.prev->next == &to_add);

    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);

    if (old_before_prev == &list.head) {
        assert(list.head.next == &to_add);
    } else {
        assert(list.head.next == old_head_next);
    }

    if (before == &list.tail) {
        assert(list.tail.prev == &to_add);
    } else {
        assert(list.tail.prev == old_tail_prev);
    }

    assert(aws_linked_list_node_is_in_list(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(old_before_prev));
    assert(aws_linked_list_node_prev_is_valid(before));

    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
