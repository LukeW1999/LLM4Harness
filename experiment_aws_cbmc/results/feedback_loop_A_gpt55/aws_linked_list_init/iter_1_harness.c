#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;

    aws_linked_list_node_reset(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(!aws_linked_list_node_is_in_list(&node));
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

void aws_linked_list_empty_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

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

void aws_linked_list_begin_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

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
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

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
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

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
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

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
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&list.head));

    struct aws_linked_list_node *old_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    struct aws_linked_list_node *result = aws_linked_list_next(&list.head);

    assert(result == old_next);
    assert(result == list.head.next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.prev == old_tail_prev);
    assert(list.tail.next == old_tail_next);
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
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    struct aws_linked_list_node *result = aws_linked_list_prev(&list.tail);

    assert(result == old_prev);
    assert(result == list.tail.prev);
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_node_prev_is_valid(&list.tail));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    aws_linked_list_init(&list);
    aws_linked_list_node_reset(&node);

    struct aws_linked_list_node *old_after_next = list.head.next;
    struct aws_linked_list_node *old_after_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    aws_linked_list_insert_after(&list.head, &node);

    assert(list.head.next == &node);
    assert(list.head.prev == old_after_prev);
    assert(node.prev == &list.head);
    assert(node.next == old_after_next);
    assert(list.tail.prev == &node);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_node_next_is_valid(&list.head));
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    aws_linked_list_init(&list);
    aws_linked_list_node_reset(&node);

    struct aws_linked_list_node *old_before_prev = list.tail.prev;
    struct aws_linked_list_node *old_before_next = list.tail.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;

    aws_linked_list_insert_before(&list.tail, &node);

    assert(list.tail.prev == &node);
    assert(list.tail.next == old_before_next);
    assert(node.next == &list.tail);
    assert(node.prev == old_before_prev);
    assert(list.head.next == &node);
    assert(list.head.prev == old_head_prev);
    assert(aws_linked_list_node_prev_is_valid(&list.tail));
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_remove_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *node = list.head.next;
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    aws_linked_list_remove(node);

    assert(node->next == NULL);
    assert(node->prev == NULL);
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);
    assert(!aws_linked_list_node_is_in_list(node));
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
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    aws_linked_list_push_back(&list, &node);

    assert(list.tail.prev == &node);
    assert(node.prev == old_last);
    assert(node.next == &list.tail);
    assert(old_last->next == &node);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    if (old_empty) {
        assert(list.head.next == &node);
    } else {
        assert(list.head.next == old_first);
    }
    assert(aws_linked_list_node_is_in_list(&node));
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
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    aws_linked_list_push_front(&list, &node);

    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == old_first);
    assert(old_first->prev == &node);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    if (old_empty) {
        assert(list.tail.prev == &node);
    } else {
        assert(list.tail.prev == old_last);
    }
    assert(aws_linked_list_node_is_in_list(&node));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
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

void aws_linked_list_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
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

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_back = list.tail.prev;
    struct aws_linked_list_node *old_before_back = old_back->prev;

    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    assert(result == old_back);
    assert(result->next == NULL);
    assert(result->prev == NULL);
    assert(!aws_linked_list_node_is_in_list(result));
    assert(list.tail.prev == old_before_back);
    assert(old_before_back->next == &list.tail);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_after_front = old_front->next;

    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    assert(result == old_front);
    assert(result->next == NULL);
    assert(result->prev == NULL);
    assert(!aws_linked_list_node_is_in_list(result));
    assert(list.head.next == old_after_front);
    assert(old_after_front->prev == &list.head);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_swap_nodes_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node a;
    struct aws_linked_list_node b;

    aws_linked_list_init(&list);
    aws_linked_list_push_back(&list, &a);
    aws_linked_list_push_back(&list, &b);

    aws_linked_list_swap_nodes(&a, &b);

    assert(list.head.next == &b);
    assert(b.prev == &list.head);
    assert(b.next == &a);
    assert(a.prev == &b);
    assert(a.next == &list.tail);
    assert(list.tail.prev == &a);
    assert(aws_linked_list_node_is_in_list(&a));
    assert(aws_linked_list_node_is_in_list(&b));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(&a != &b);
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    bool old_a_empty = aws_linked_list_empty(&a);
    bool old_b_empty = aws_linked_list_empty(&b);
    struct aws_linked_list_node *old_a_first = a.head.next;
    struct aws_linked_list_node *old_a_last = a.tail.prev;
    struct aws_linked_list_node *old_b_first = b.head.next;
    struct aws_linked_list_node *old_b_last = b.tail.prev;

    aws_linked_list_swap_contents(&a, &b);

    if (old_b_empty) {
        assert(aws_linked_list_empty(&a));
    } else {
        assert(a.head.next == old_b_first);
        assert(a.tail.prev == old_b_last);
        assert(old_b_first->prev == &a.head);
        assert(old_b_last->next == &a.tail);
    }

    if (old_a_empty) {
        assert(aws_linked_list_empty(&b));
    } else {
        assert(b.head.next == old_a_first);
        assert(b.tail.prev == old_a_last);
        assert(old_a_first->prev == &b.head);
        assert(old_a_last->next == &b.tail);
    }

    assert(a.head.prev == NULL);
    assert(a.tail.next == NULL);
    assert(b.head.prev == NULL);
    assert(b.tail.next == NULL);
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
}

void aws_linked_list_move_all_back_harness() {
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(&dst != &src);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    bool old_src_empty = aws_linked_list_empty(&src);
    bool old_dst_empty = aws_linked_list_empty(&dst);
    struct aws_linked_list_node *old_dst_front = dst.head.next;
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (old_src_empty) {
        assert(dst.head.next == old_dst_front);
        assert(dst.tail.prev == old_dst_back);
    } else {
        if (old_dst_empty) {
            assert(dst.head.next == old_src_front);
        } else {
            assert(dst.head.next == old_dst_front);
            assert(old_dst_back->next == old_src_front);
            assert(old_src_front->prev == old_dst_back);
        }
        assert(dst.tail.prev == old_src_back);
        assert(old_src_back->next == &dst.tail);
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
    __CPROVER_assume(&dst != &src);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    bool old_src_empty = aws_linked_list_empty(&src);
    bool old_dst_empty = aws_linked_list_empty(&dst);
    struct aws_linked_list_node *old_dst_front = dst.head.next;
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    aws_linked_list_move_all_front(&dst, &src);

    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (old_src_empty) {
        assert(dst.head.next == old_dst_front);
        assert(dst.tail.prev == old_dst_back);
    } else {
        assert(dst.head.next == old_src_front);
        assert(old_src_front->prev == &dst.head);
        if (old_dst_empty) {
            assert(dst.tail.prev == old_src_back);
        } else {
            assert(dst.tail.prev == old_dst_back);
            assert(old_src_back->next == old_dst_front);
            assert(old_dst_front->prev == old_src_back);
        }
    }
