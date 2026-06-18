#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_node_reset(&node);

    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&list.tail));

    bool old_empty = aws_linked_list_empty(&list);
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    aws_linked_list_push_back(&list, &node);

    assert(aws_linked_list_is_valid(&list));
    assert(!aws_linked_list_empty(&list));

    assert(list.tail.prev == &node);
    assert(node.prev == old_tail_prev);
    assert(node.next == &list.tail);
    assert(old_tail_prev->next == &node);

    if (old_empty) {
        assert(list.head.next == &node);
        assert(node.prev == &list.head);
    } else {
        assert(list.head.next == old_head_next);
    }

    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);

    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
}

void aws_linked_list_push_front_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_node_reset(&node);

    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&list.head));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(list.head.next));

    bool old_empty = aws_linked_list_empty(&list);
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    aws_linked_list_push_front(&list, &node);

    assert(aws_linked_list_is_valid(&list));
    assert(!aws_linked_list_empty(&list));

    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == old_head_next);
    assert(old_head_next->prev == &node);

    if (old_empty) {
        assert(list.tail.prev == &node);
        assert(node.next == &list.tail);
    } else {
        assert(list.tail.prev == old_tail_prev);
    }

    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);

    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
}

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list dst;
    struct aws_linked_list src;
    struct aws_linked_list_node dst_node;
    struct aws_linked_list_node src_node_1;
    struct aws_linked_list_node src_node_2;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    aws_linked_list_init(&dst);
    aws_linked_list_init(&src);
    aws_linked_list_node_reset(&dst_node);
    aws_linked_list_node_reset(&src_node_1);
    aws_linked_list_node_reset(&src_node_2);

    bool dst_has_node = nondet_bool();
    bool src_has_node = nondet_bool();
    bool src_has_second_node = nondet_bool();

    if (dst_has_node) {
        aws_linked_list_push_back(&dst, &dst_node);
    }

    if (src_has_node) {
        aws_linked_list_push_back(&src, &src_node_1);
        if (src_has_second_node) {
            aws_linked_list_push_back(&src, &src_node_2);
        }
    }

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    bool old_dst_empty = aws_linked_list_empty(&dst);
    bool old_src_empty = aws_linked_list_empty(&src);

    struct aws_linked_list_node *old_dst_front = dst.head.next;
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    struct aws_linked_list_node *old_dst_head_prev = dst.head.prev;
    struct aws_linked_list_node *old_dst_tail_next = dst.tail.next;
    struct aws_linked_list_node *old_src_head_prev = src.head.prev;
    struct aws_linked_list_node *old_src_tail_next = src.tail.next;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_empty(&src));

    assert(dst.head.prev == old_dst_head_prev);
    assert(dst.tail.next == old_dst_tail_next);
    assert(src.head.prev == old_src_head_prev);
    assert(src.tail.next == old_src_tail_next);

    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    if (old_src_empty) {
        assert(dst.head.next == old_dst_front);
        assert(dst.tail.prev == old_dst_back);
        if (old_dst_empty) {
            assert(aws_linked_list_empty(&dst));
        } else {
            assert(dst_node.prev == &dst.head);
            assert(dst_node.next == &dst.tail);
        }
    } else {
        assert(!aws_linked_list_empty(&dst));
        assert(dst.tail.prev == old_src_back);
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);
        assert(old_src_back->next == &dst.tail);

        if (old_dst_empty) {
            assert(dst.head.next == old_src_front);
            assert(old_src_front->prev == &dst.head);
        } else {
            assert(dst.head.next == old_dst_front);
            assert(dst_node.prev == &dst.head);
            assert(dst_node.next == old_src_front);
        }

        if (src_has_second_node) {
            assert(src_node_1.next == &src_node_2);
            assert(src_node_2.prev == &src_node_1);
        }
    }
}

void aws_linked_list_move_all_front_harness(void) {
    struct aws_linked_list dst;
    struct aws_linked_list src;
    struct aws_linked_list_node dst_node;
    struct aws_linked_list_node src_node_1;
    struct aws_linked_list_node src_node_2;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    aws_linked_list_init(&dst);
    aws_linked_list_init(&src);
    aws_linked_list_node_reset(&dst_node);
    aws_linked_list_node_reset(&src_node_1);
    aws_linked_list_node_reset(&src_node_2);

    bool dst_has_node = nondet_bool();
    bool src_has_node = nondet_bool();
    bool src_has_second_node = nondet_bool();

    if (dst_has_node) {
        aws_linked_list_push_back(&dst, &dst_node);
    }

    if (src_has_node) {
        aws_linked_list_push_back(&src, &src_node_1);
        if (src_has_second_node) {
            aws_linked_list_push_back(&src, &src_node_2);
        }
    }

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    bool old_dst_empty = aws_linked_list_empty(&dst);
    bool old_src_empty = aws_linked_list_empty(&src);

    struct aws_linked_list_node *old_dst_front = dst.head.next;
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    struct aws_linked_list_node *old_dst_head_prev = dst.head.prev;
    struct aws_linked_list_node *old_dst_tail_next = dst.tail.next;
    struct aws_linked_list_node *old_src_head_prev = src.head.prev;
    struct aws_linked_list_node *old_src_tail_next = src.tail.next;

    aws_linked_list_move_all_front(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_empty(&src));

    assert(dst.head.prev == old_dst_head_prev);
    assert(dst.tail.next == old_dst_tail_next);
    assert(src.head.prev == old_src_head_prev);
    assert(src.tail.next == old_src_tail_next);

    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    if (old_src_empty) {
        assert(dst.head.next == old_dst_front);
        assert(dst.tail.prev == old_dst_back);
        if (old_dst_empty) {
            assert(aws_linked_list_empty(&dst));
        } else {
            assert(dst_node.prev == &dst.head);
            assert(dst_node.next == &dst.tail);
        }
    } else {
        assert(!aws_linked_list_empty(&dst));
        assert(dst.head.next == old_src_front);
        assert(old_src_front->prev == &dst.head);
        assert(old_src_back->next == old_dst_front);
        assert(old_dst_front->prev == old_src_back);

        if (old_dst_empty) {
            assert(dst.tail.prev == old_src_back);
            assert(old_src_back->next == &dst.tail);
        } else {
            assert(dst.tail.prev == old_dst_back);
            assert(dst_node.prev == old_src_back);
            assert(dst_node.next == &dst.tail);
        }

        if (src_has_second_node) {
            assert(src_node_1.next == &src_node_2);
            assert(src_node_2.prev == &src_node_1);
        }
    }
}
