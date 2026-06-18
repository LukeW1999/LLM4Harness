#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

static void assume_bounded_valid_linked_list(struct aws_linked_list *list) {
    ensure_linked_list_is_allocated(list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(list));
}

static void initialize_empty_linked_list(struct aws_linked_list *list) {
    ensure_linked_list_is_allocated(list, 0);
    aws_linked_list_init(list);
    assert(aws_linked_list_is_valid(list));
    assert(aws_linked_list_is_valid_deep(list));
    assert(aws_linked_list_empty(list));
}

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;

    aws_linked_list_node_reset(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(!aws_linked_list_node_is_in_list(&node));
}

void aws_linked_list_init_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    aws_linked_list_init(&list);

    assert(list.head.next == &list.tail);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_empty(&list));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_empty_harness() {
    struct aws_linked_list list;
    assume_bounded_valid_linked_list(&list);

    struct aws_linked_list old = list;

    bool result = aws_linked_list_empty(&list);

    assert(result == (old.head.next == &old.tail));
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_begin_harness() {
    struct aws_linked_list list;
    assume_bounded_valid_linked_list(&list);

    struct aws_linked_list old = list;

    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    assert(result == old.head.next);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_end_harness() {
    struct aws_linked_list list;
    assume_bounded_valid_linked_list(&list);

    struct aws_linked_list old = list;

    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    assert(result == &list.tail);
    assert(result == &old.tail);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_rbegin_harness() {
    struct aws_linked_list list;
    assume_bounded_valid_linked_list(&list);

    struct aws_linked_list old = list;

    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    assert(result == old.tail.prev);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_rend_harness() {
    struct aws_linked_list list;
    assume_bounded_valid_linked_list(&list);

    struct aws_linked_list old = list;

    const struct aws_linked_list_node *result = aws_linked_list_rend(&list);

    assert(result == &list.head);
    assert(result == &old.head);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_next_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node next;

    node.next = &next;
    next.prev = &node;
    node.prev = NULL;
    next.next = NULL;

    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node));

    struct aws_linked_list_node old_node = node;
    struct aws_linked_list_node old_next = next;

    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    assert(result == &next);
    assert(result == old_node.next);
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(node.next == old_node.next);
    assert(node.prev == old_node.prev);
    assert(next.next == old_next.next);
    assert(next.prev == old_next.prev);
}

void aws_linked_list_prev_harness() {
    struct aws_linked_list_node prev;
    struct aws_linked_list_node node;

    prev.next = &node;
    node.prev = &prev;
    prev.prev = NULL;
    node.next = NULL;

    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&node));

    struct aws_linked_list_node old_prev = prev;
    struct aws_linked_list_node old_node = node;

    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    assert(result == &prev);
    assert(result == old_node.prev);
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(prev.next == old_prev.next);
    assert(prev.prev == old_prev.prev);
    assert(node.next == old_node.next);
    assert(node.prev == old_node.prev);
}

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    initialize_empty_linked_list(&list);
    aws_linked_list_node_reset(&node);

    struct aws_linked_list_node *after = &list.head;
    struct aws_linked_list_node *old_after_next = after->next;

    aws_linked_list_insert_after(after, &node);

    assert(after->next == &node);
    assert(node.prev == after);
    assert(node.next == old_after_next);
    assert(old_after_next->prev == &node);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_next_is_valid(after));
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    initialize_empty_linked_list(&list);
    aws_linked_list_node_reset(&node);

    struct aws_linked_list_node *before = &list.tail;
    struct aws_linked_list_node *old_before_prev = before->prev;

    aws_linked_list_insert_before(before, &node);

    assert(before->prev == &node);
    assert(node.next == before);
    assert(node.prev == old_before_prev);
    assert(old_before_prev->next == &node);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_prev_is_valid(before));
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    assume_bounded_valid_linked_list(&list);
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
        assert(old_first->prev == &list.head);
    }
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_is_in_list(&node));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    assume_bounded_valid_linked_list(&list);
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
        assert(old_last->next == &list.tail);
    }
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_is_in_list(&node));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    initialize_empty_linked_list(&list);
    aws_linked_list_push_back(&list, &node);

    struct aws_linked_list old = list;
    struct aws_linked_list_node old_node = node;

    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    assert(result == &node);
    assert(result == old.tail.prev);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);
    assert(node.next == old_node.next);
    assert(node.prev == old_node.prev);
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    initialize_empty_linked_list(&list);
    aws_linked_list_push_front(&list, &node);

    struct aws_linked_list old = list;
    struct aws_linked_list_node old_node = node;

    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    assert(result == &node);
    assert(result == old.head.next);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);
    assert(node.next == old_node.next);
    assert(node.prev == old_node.prev);
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_remove_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    initialize_empty_linked_list(&list);
    aws_linked_list_push_back(&list, &node);

    aws_linked_list_remove(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(list.head.next == &list.tail);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_empty(&list));
    assert(!aws_linked_list_node_is_in_list(&node));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    initialize_empty_linked_list(&list);
    aws_linked_list_push_back(&list, &node);

    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    assert(result == &node);
    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(list.head.next == &list.tail);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_empty(&list));
    assert(!aws_linked_list_node_is_in_list(&node));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    initialize_empty_linked_list(&list);
    aws_linked_list_push_front(&list, &node);

    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    assert(result == &node);
    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(list.head.next == &list.tail);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_empty(&list));
    assert(!aws_linked_list_node_is_in_list(&node));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_swap_nodes_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node a;
    struct aws_linked_list_node b;

    initialize_empty_linked_list(&list);
    aws_linked_list_push_back(&list, &a);
    aws_linked_list_push_back(&list, &b);

    aws_linked_list_swap_nodes(&a, &b);

    assert(list.head.next == &b);
    assert(b.prev == &list.head);
    assert(b.next == &a);
    assert(a.prev == &b);
    assert(a.next == &list.tail);
    assert(list.tail.prev == &a);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_prev_is_valid(&a));
    assert(aws_linked_list_node_next_is_valid(&a));
    assert(aws_linked_list_node_prev_is_valid(&b));
    assert(aws_linked_list_node_next_is_valid(&b));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    struct aws_linked_list b;
    struct aws_linked_list_node a_node;
    struct aws_linked_list_node b_node;

    initialize_empty_linked_list(&a);
    initialize_empty_linked_list(&b);

    bool a_has_node = nondet_bool();
    bool b_has_node = nondet_bool();

    if (a_has_node) {
        aws_linked_list_push_back(&a, &a_node);
    }
    if (b_has_node) {
        aws_linked_list_push_back(&b, &b_node);
    }

    bool old_a_empty = aws_linked_list_empty(&a);
    bool old_b_empty = aws_linked_list_empty(&b);
    struct aws_linked_list_node *old_a_first = a.head.next;
    struct aws_linked_list_node *old_a_last = a.tail.prev;
    struct aws_linked_list_node *old_b_first = b.head.next;
    struct aws_linked_list_node *old_b_last = b.tail.prev;

    aws_linked_list_swap_contents(&a, &b);

    assert(aws_linked_list_empty(&a) == old_b_empty);
    assert(aws_linked_list_empty(&b) == old_a_empty);

    if (old_b_empty) {
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        assert(a.head.next == old_b_first);
        assert(a.tail.prev == old_b_last);
        assert(old_b_first->prev == &a.head);
        assert(old_b_last->next == &a.tail);
    }

    if (old_a_empty) {
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
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
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));
}

void aws_linked_list_move_all_back_harness() {
    struct aws_linked_list dst;
    struct aws_linked_list src;
    struct aws_linked_list_node dst_node;
    struct aws_linked_list_node src_node;

    initialize_empty_linked_list(&dst);
    initialize_empty_linked_list(&src);

    bool dst_has_node = nondet_bool();
    bool src_has_node = nondet_bool();

    if (dst_has_node) {
        aws_linked_list_push_back(&dst, &dst_node);
    }
    if (src_has_node) {
        aws_linked_list_push_back(&src, &src_node);
    }

    bool old_dst_empty = aws_linked_list_empty(&dst);
    bool old_src_empty = aws_linked_list_empty(&src);
    struct aws_linked_list_node *old_dst_first = dst.head.next;
    struct aws_linked_list_node *old_dst_last = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.head.prev == NULL);
    assert(src.tail.prev == &src.head);
    assert(src.tail.next == NULL);

    if (old_src_empty) {
        assert(aws_linked_list_empty(&dst) == old_dst_empty);
        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_dst_last);
    } else {
        assert(dst.tail.prev == old_src_back);
        assert(old_src_back->next == &dst.tail);
        if (old_dst_empty) {
            assert(dst.head.next == old_src_front);
            assert(old_src_front->prev == &dst.head);
        } else {
            assert(dst.head.next == old_dst_first);
            assert(old_dst_last->next == old_src_front);
            assert(old_src_front->prev == old_dst_last);
        }
    }

    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid_deep(&dst));
    assert(aws_linked_list_is_valid_deep(&src));
}
