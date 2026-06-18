#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;

    aws_linked_list_node_reset(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
}

void aws_linked_list_empty_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    bool result = aws_linked_list_empty(&list);

    assert(result == (list.head.next == &list.tail));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_begin_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    assert(result == old_head_next);
    assert(list.head.next == old_head_next);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == old_tail_prev);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_end_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    assert(result == &list.tail);
    assert(list.head.next == old_head_next);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == old_tail_prev);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_rbegin_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    assert(result == old_tail_prev);
    assert(list.head.next == old_head_next);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == old_tail_prev);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_rend_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    const struct aws_linked_list_node *result = aws_linked_list_rend(&list);

    assert(result == &list.head);
    assert(list.head.next == old_head_next);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == old_tail_prev);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_next_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&list.head));

    struct aws_linked_list_node *old_next = list.head.next;

    struct aws_linked_list_node *result = aws_linked_list_next(&list.head);

    assert(result == old_next);
    assert(list.head.next == old_next);
    assert(aws_linked_list_node_next_is_valid(&list.head));
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_prev_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&list.tail));

    struct aws_linked_list_node *old_prev = list.tail.prev;

    struct aws_linked_list_node *result = aws_linked_list_prev(&list.tail);

    assert(result == old_prev);
    assert(list.tail.prev == old_prev);
    assert(aws_linked_list_node_prev_is_valid(&list.tail));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node to_add;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    aws_linked_list_node_reset(&to_add);

    struct aws_linked_list_node *after = list.tail.prev;
    __CPROVER_assume(aws_linked_list_node_next_is_valid(after));

    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_head_next = list.head.next;

    aws_linked_list_insert_after(after, &to_add);

    assert(after->next == &to_add);
    assert(to_add.prev == after);
    assert(to_add.next == old_after_next);
    assert(old_after_next->prev == &to_add);
    assert(aws_linked_list_node_next_is_valid(after));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));
    if (after != &list.head) {
        assert(list.head.next == old_head_next);
    }
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node to_add;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    aws_linked_list_node_reset(&to_add);

    struct aws_linked_list_node *before = list.head.next;
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(before));

    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    aws_linked_list_insert_before(before, &to_add);

    assert(before->prev == &to_add);
    assert(to_add.next == before);
    assert(to_add.prev == old_before_prev);
    assert(old_before_prev->next == &to_add);
    assert(aws_linked_list_node_prev_is_valid(before));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));
    if (before != &list.tail) {
        assert(list.tail.prev == old_tail_prev);
    }
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_remove_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *node = list.head.next;
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));

    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    aws_linked_list_remove(node);

    assert(node->next == NULL);
    assert(node->prev == NULL);
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    aws_linked_list_node_reset(&node);

    bool old_empty = aws_linked_list_empty(&list);
    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;

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
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;

    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    assert(result == old_last);
    assert(list.head.next == old_first);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == old_last);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_back = list.tail.prev;
    struct aws_linked_list_node *old_prev = old_back->prev;
    struct aws_linked_list_node *old_first = list.head.next;
    bool old_singleton = old_first == old_back;

    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    assert(result == old_back);
    assert(result->next == NULL);
    assert(result->prev == NULL);
    assert(list.tail.prev == old_prev);
    assert(old_prev->next == &list.tail);
    if (old_singleton) {
        assert(aws_linked_list_empty(&list));
        assert(list.head.next == &list.tail);
    } else {
        assert(list.head.next == old_first);
    }
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    aws_linked_list_node_reset(&node);

    bool old_empty = aws_linked_list_empty(&list);
    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;

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
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;

    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    assert(result == old_first);
    assert(list.head.next == old_first);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == old_last);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_next = old_front->next;
    struct aws_linked_list_node *old_last = list.tail.prev;
    bool old_singleton = old_last == old_front;

    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    assert(result == old_front);
    assert(result->next == NULL);
    assert(result->prev == NULL);
    assert(list.head.next == old_next);
    assert(old_next->prev == &list.head);
    if (old_singleton) {
        assert(aws_linked_list_empty(&list));
        assert(list.tail.prev == &list.head);
    } else {
        assert(list.tail.prev == old_last);
    }
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_move_all_back_harness() {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    bool src_was_empty = aws_linked_list_empty(&src);
    struct aws_linked_list_node *dst_old_front = dst.head.next;
    struct aws_linked_list_node *dst_old_back = dst.tail.prev;
    struct aws_linked_list_node *src_old_front = src.head.next;
    struct aws_linked_list_node *src_old_back = src.tail.prev;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.head.prev == NULL);
    assert(src.tail.prev == &src.head);
    assert(src.tail.next == NULL);

    if (src_was_empty) {
        assert(dst.head.next == dst_old_front);
        assert(dst.tail.prev == dst_old_back);
    } else {
        assert(dst_old_back->next == src_old_front);
        assert(src_old_front->prev == dst_old_back);
        assert(dst.tail.prev == src_old_back);
        assert(src_old_back->next == &dst.tail);
    }

    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));
}

void aws_linked_list_move_all_front_harness() {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    bool src_was_empty = aws_linked_list_empty(&src);
    struct aws_linked_list_node *dst_old_front = dst.head.next;
    struct aws_linked_list_node *dst_old_back = dst.tail.prev;
    struct aws_linked_list_node *src_old_front = src.head.next;
    struct aws_linked_list_node *src_old_back = src.tail.prev;

    aws_linked_list_move_all_front(&dst, &src);

    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.head.prev == NULL);
    assert(src.tail.prev == &src.head);
    assert(src.tail.next == NULL);

    if (src_was_empty) {
        assert(dst.head.next == dst_old_front);
        assert(dst.tail.prev == dst_old_back);
    } else {
        assert(dst.head.next == src_old_front);
        assert(src_old_front->prev == &dst.head);
        assert(src_old_back->next == dst_old_front);
        assert(dst_old_front->prev == src_old_back);
        if (dst_old_front == &dst.tail) {
            assert(dst.tail.prev == src_old_back);
        } else {
            assert(dst.tail.prev == dst_old_back);
        }
    }

    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));
}

void aws_linked_list_node_is_in_list_harness() {
    struct aws_linked_list_node node;

    if (nondet_bool()) {
        struct aws_linked_list list;
        ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
        __CPROVER_assume(aws_linked_list_is_valid(&list));

        aws_linked_list_node_reset(&node);
        aws_linked_list_push_back(&list, &node);

        bool result = aws_linked_list_node_is_in_list(&node);

        assert(result == true);
        assert(aws_linked_list_node_prev_is_valid(&node));
        assert(aws_linked_list_node_next_is_valid(&node));
        assert(aws_linked_list_is_valid(&list));
    } else {
        aws_linked_list_node_reset(&node);

        bool result = aws_linked_list_node_is_in_list(&node);

        assert(result == false);
        assert(node.next == NULL);
        assert(node.prev == NULL);
    }
}
