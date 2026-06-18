#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static void ensure_valid_linked_list_for_harness(struct aws_linked_list *list) {
    ensure_linked_list_is_allocated(list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(list));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&list->head));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&list->tail));
}

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node node;

    aws_linked_list_node_reset(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
}

void aws_linked_list_empty_harness(void) {
    struct aws_linked_list list;
    ensure_valid_linked_list_for_harness(&list);

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    bool result = aws_linked_list_empty(&list);

    assert(result == (old_head_next == &list.tail));
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.prev == old_tail_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_begin_harness(void) {
    struct aws_linked_list list;
    ensure_valid_linked_list_for_harness(&list);

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    assert(result == old_head_next);
    assert(result == list.head.next);
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.prev == old_tail_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_end_harness(void) {
    struct aws_linked_list list;
    ensure_valid_linked_list_for_harness(&list);

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    assert(result == &list.tail);
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.prev == old_tail_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_rbegin_harness(void) {
    struct aws_linked_list list;
    ensure_valid_linked_list_for_harness(&list);

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    assert(result == old_tail_prev);
    assert(result == list.tail.prev);
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.prev == old_tail_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_rend_harness(void) {
    struct aws_linked_list list;
    ensure_valid_linked_list_for_harness(&list);

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    const struct aws_linked_list_node *result = aws_linked_list_rend(&list);

    assert(result == &list.head);
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.prev == old_tail_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_next_harness(void) {
    struct aws_linked_list list;
    ensure_valid_linked_list_for_harness(&list);

    struct aws_linked_list_node *node = &list.head;
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node *old_prev = node->prev;

    struct aws_linked_list_node *result = aws_linked_list_next(node);

    assert(result == old_next);
    assert(result == node->next);
    assert(node->prev == old_prev);
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_prev_harness(void) {
    struct aws_linked_list list;
    ensure_valid_linked_list_for_harness(&list);

    struct aws_linked_list_node *node = &list.tail;
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node *old_prev = node->prev;

    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    assert(result == old_prev);
    assert(result == node->prev);
    assert(node->next == old_next);
    assert(aws_linked_list_node_prev_is_valid(node));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_insert_after_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node to_add;

    ensure_valid_linked_list_for_harness(&list);
    aws_linked_list_node_reset(&to_add);

    struct aws_linked_list_node *after = &list.head;
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    aws_linked_list_insert_after(after, &to_add);

    assert(after->next == &to_add);
    assert(to_add.prev == after);
    assert(to_add.next == old_after_next);
    assert(old_after_next->prev == &to_add);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_node_next_is_valid(after));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_insert_before_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node to_add;

    ensure_valid_linked_list_for_harness(&list);
    aws_linked_list_node_reset(&to_add);

    struct aws_linked_list_node *before = &list.tail;
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    aws_linked_list_insert_before(before, &to_add);

    assert(before->prev == &to_add);
    assert(to_add.next == before);
    assert(to_add.prev == old_before_prev);
    assert(old_before_prev->next == &to_add);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_node_prev_is_valid(before));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_push_back_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    ensure_valid_linked_list_for_harness(&list);
    aws_linked_list_node_reset(&node);

    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    bool old_empty = aws_linked_list_empty(&list);

    aws_linked_list_push_back(&list, &node);

    assert(list.tail.prev == &node);
    assert(node.prev == old_last);
    assert(node.next == &list.tail);
    assert(old_last->next == &node);
    if (old_empty) {
        assert(list.head.next == &node);
    } else {
        assert(list.head.next == old_first);
    }
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(!aws_linked_list_empty(&list));
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_push_front_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    ensure_valid_linked_list_for_harness(&list);
    aws_linked_list_node_reset(&node);

    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    bool old_empty = aws_linked_list_empty(&list);

    aws_linked_list_push_front(&list, &node);

    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == old_first);
    assert(old_first->prev == &node);
    if (old_empty) {
        assert(list.tail.prev == &node);
    } else {
        assert(list.tail.prev == old_last);
    }
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(!aws_linked_list_empty(&list));
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_back_harness(void) {
    struct aws_linked_list list;
    ensure_valid_linked_list_for_harness(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(list.tail.prev));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(list.tail.prev));

    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    assert(result == old_last);
    assert(list.head.next == old_first);
    assert(list.tail.prev == old_last);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_front_harness(void) {
    struct aws_linked_list list;
    ensure_valid_linked_list_for_harness(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(list.head.next));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(list.head.next));

    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    assert(result == old_first);
    assert(list.head.next == old_first);
    assert(list.tail.prev == old_last);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_remove_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    aws_linked_list_init(&list);
    aws_linked_list_node_reset(&node);
    aws_linked_list_push_back(&list, &node);

    aws_linked_list_remove(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(list.head.next == &list.tail);
    assert(list.tail.prev == &list.head);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_empty(&list));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_pop_back_harness(void) {
    struct aws_linked_list list;
    ensure_valid_linked_list_for_harness(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(list.tail.prev));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(list.tail.prev));

    struct aws_linked_list_node *old_back = list.tail.prev;
    struct aws_linked_list_node *old_back_prev = old_back->prev;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    assert(result == old_back);
    assert(old_back->next == NULL);
    assert(old_back->prev == NULL);
    assert(list.tail.prev == old_back_prev);
    assert(old_back_prev->next == &list.tail);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_pop_front_harness(void) {
    struct aws_linked_list list;
    ensure_valid_linked_list_for_harness(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(list.head.next));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(list.head.next));

    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_front_next = old_front->next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    assert(result == old_front);
    assert(old_front->next == NULL);
    assert(old_front->prev == NULL);
    assert(list.head.next == old_front_next);
    assert(old_front_next->prev == &list.head);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_is_valid(&list));
}
