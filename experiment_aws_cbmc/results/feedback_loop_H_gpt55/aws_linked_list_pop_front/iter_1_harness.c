#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static void assume_valid_allocated_linked_list(struct aws_linked_list *list) {
    ensure_linked_list_is_allocated(list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(list));
}

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;

    aws_linked_list_node_reset(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(!aws_linked_list_node_is_in_list(&node));
}

void aws_linked_list_empty_harness() {
    struct aws_linked_list list;
    assume_valid_allocated_linked_list(&list);

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

void aws_linked_list_init_harness() {
    struct aws_linked_list list;

    aws_linked_list_init(&list);

    assert(list.head.next == &list.tail);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_empty(&list));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_begin_harness() {
    struct aws_linked_list list;
    assume_valid_allocated_linked_list(&list);

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

void aws_linked_list_end_harness() {
    struct aws_linked_list list;
    assume_valid_allocated_linked_list(&list);

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

void aws_linked_list_rbegin_harness() {
    struct aws_linked_list list;
    assume_valid_allocated_linked_list(&list);

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

void aws_linked_list_rend_harness() {
    struct aws_linked_list list;
    assume_valid_allocated_linked_list(&list);

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

void aws_linked_list_next_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node next;
    struct aws_linked_list_node old_node = node;
    struct aws_linked_list_node old_next = next;

    node.next = &next;
    next.prev = &node;

    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    assert(result == &next);
    assert(result == node.next);
    assert(node.next == &next);
    assert(node.prev == old_node.prev);
    assert(next.prev == &node);
    assert(next.next == old_next.next);
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_node_prev_is_valid(result));
}

void aws_linked_list_prev_harness() {
    struct aws_linked_list_node prev;
    struct aws_linked_list_node node;
    struct aws_linked_list_node old_prev = prev;
    struct aws_linked_list_node old_node = node;

    node.prev = &prev;
    prev.next = &node;

    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    assert(result == &prev);
    assert(result == node.prev);
    assert(node.prev == &prev);
    assert(node.next == old_node.next);
    assert(prev.next == &node);
    assert(prev.prev == old_prev.prev);
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(result));
}

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list_node after;
    struct aws_linked_list_node before;
    struct aws_linked_list_node to_add;

    struct aws_linked_list_node *old_after_prev = after.prev;
    struct aws_linked_list_node *old_before_next = before.next;

    after.next = &before;
    before.prev = &after;

    aws_linked_list_insert_after(&after, &to_add);

    assert(after.next == &to_add);
    assert(after.prev == old_after_prev);
    assert(to_add.prev == &after);
    assert(to_add.next == &before);
    assert(before.prev == &to_add);
    assert(before.next == old_before_next);
    assert(aws_linked_list_node_next_is_valid(&after));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(aws_linked_list_node_prev_is_valid(&before));
}

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list_node after;
    struct aws_linked_list_node before;
    struct aws_linked_list_node to_add;

    struct aws_linked_list_node *old_after_prev = after.prev;
    struct aws_linked_list_node *old_before_next = before.next;

    after.next = &before;
    before.prev = &after;

    aws_linked_list_insert_before(&before, &to_add);

    assert(before.prev == &to_add);
    assert(before.next == old_before_next);
    assert(to_add.prev == &after);
    assert(to_add.next == &before);
    assert(after.next == &to_add);
    assert(after.prev == old_after_prev);
    assert(aws_linked_list_node_prev_is_valid(&before));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&after));
}

void aws_linked_list_remove_harness() {
    struct aws_linked_list_node prev;
    struct aws_linked_list_node node;
    struct aws_linked_list_node next;

    struct aws_linked_list_node *old_prev_prev = prev.prev;
    struct aws_linked_list_node *old_next_next = next.next;

    prev.next = &node;
    node.prev = &prev;
    node.next = &next;
    next.prev = &node;

    aws_linked_list_remove(&node);

    assert(prev.next == &next);
    assert(prev.prev == old_prev_prev);
    assert(next.prev == &prev);
    assert(next.next == old_next_next);
    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(aws_linked_list_node_next_is_valid(&prev));
    assert(aws_linked_list_node_prev_is_valid(&next));
    assert(!aws_linked_list_node_is_in_list(&node));
}

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    assume_valid_allocated_linked_list(&list);

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    bool was_empty = aws_linked_list_empty(&list);

    aws_linked_list_push_back(&list, &node);

    assert(list.tail.prev == &node);
    assert(node.prev == old_tail_prev);
    assert(node.next == &list.tail);
    assert(old_tail_prev->next == &node);
    if (was_empty) {
        assert(list.head.next == &node);
    } else {
        assert(list.head.next == old_head_next);
    }
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_back_harness() {
    struct aws_linked_list list;
    assume_valid_allocated_linked_list(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    assert(result == old_tail_prev);
    assert(result == list.tail.prev);
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.prev == old_tail_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    assume_valid_allocated_linked_list(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_back = list.tail.prev;
    struct aws_linked_list_node *old_before_back = old_back->prev;

    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    assert(result == old_back);
    assert(result->next == NULL);
    assert(result->prev == NULL);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    if (old_before_back == &list.head) {
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
        assert(aws_linked_list_empty(&list));
    } else {
        assert(list.head.next == old_head_next);
        assert(list.tail.prev == old_before_back);
        assert(old_before_back->next == &list.tail);
    }
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    assume_valid_allocated_linked_list(&list);

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    bool was_empty = aws_linked_list_empty(&list);

    aws_linked_list_push_front(&list, &node);

    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == old_head_next);
    assert(old_head_next->prev == &node);
    if (was_empty) {
        assert(list.tail.prev == &node);
    } else {
        assert(list.tail.prev == old_tail_prev);
    }
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_front_harness() {
    struct aws_linked_list list;
    assume_valid_allocated_linked_list(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    assert(result == old_head_next);
    assert(result == list.head.next);
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.prev == old_tail_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    assume_valid_allocated_linked_list(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_after_front = old_front->next;

    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    assert(result == old_front);
    assert(result->next == NULL);
    assert(result->prev == NULL);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    if (old_after_front == &list.tail) {
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
        assert(aws_linked_list_empty(&list));
    } else {
        assert(list.head.next == old_after_front);
        assert(old_after_front->prev == &list.head);
        assert(list.tail.prev == old_tail_prev);
    }
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_move_all_back_harness() {
    struct aws_linked_list dst;
    struct aws_linked_list src;
    assume_valid_allocated_linked_list(&dst);
    assume_valid_allocated_linked_list(&src);

    bool src_was_empty = aws_linked_list_empty(&src);
    struct aws_linked_list_node *old_dst_front = dst.head.next;
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_dst_head_prev = dst.head.prev;
    struct aws_linked_list_node *old_dst_tail_next = dst.tail.next;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;
    struct aws_linked_list_node *old_src_head_prev = src.head.prev;
    struct aws_linked_list_node *old_src_tail_next = src.tail.next;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.head.prev == old_src_head_prev);
    assert(src.tail.prev == &src.head);
    assert(src.tail.next == old_src_tail_next);
    assert(dst.head.prev == old_dst_head_prev);
    assert(dst.tail.next == old_dst_tail_next);

    if (src_was_empty) {
        assert(dst.head.next == old_dst_front);
        assert(dst.tail.prev == old_dst_back);
    } else {
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);
        assert(dst.tail.prev == old_src_back);
        assert(old_src_back->next == &dst.tail);
    }

    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));
}
