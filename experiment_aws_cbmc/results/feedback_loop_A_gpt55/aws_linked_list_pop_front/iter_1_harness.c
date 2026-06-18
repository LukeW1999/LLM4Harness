#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static void ensure_valid_linked_list(struct aws_linked_list *list) {
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
    ensure_valid_linked_list(&list);

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    bool result = aws_linked_list_empty(&list);

    assert(result == (list.head.next == &list.tail));
    assert(list.head.next == old_head_next);
    assert(list.tail.prev == old_tail_prev);
    assert(list.head.prev == old_head_prev);
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
    ensure_valid_linked_list(&list);

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    assert(result == old_head_next);
    assert(result == list.head.next);
    assert(list.tail.prev == old_tail_prev);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_end_harness() {
    struct aws_linked_list list;
    ensure_valid_linked_list(&list);

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    assert(result == &list.tail);
    assert(list.head.next == old_head_next);
    assert(list.tail.prev == old_tail_prev);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_rbegin_harness() {
    struct aws_linked_list list;
    ensure_valid_linked_list(&list);

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    assert(result == old_tail_prev);
    assert(result == list.tail.prev);
    assert(list.head.next == old_head_next);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_rend_harness() {
    struct aws_linked_list list;
    ensure_valid_linked_list(&list);

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    const struct aws_linked_list_node *result = aws_linked_list_rend(&list);

    assert(result == &list.head);
    assert(list.head.next == old_head_next);
    assert(list.tail.prev == old_tail_prev);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_next_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node first;
    struct aws_linked_list_node second;

    aws_linked_list_init(&list);
    aws_linked_list_push_back(&list, &first);
    aws_linked_list_push_back(&list, &second);

    struct aws_linked_list_node *result = aws_linked_list_next(&first);

    assert(result == &second);
    assert(first.next == &second);
    assert(second.prev == &first);
    assert(aws_linked_list_node_next_is_valid(&first));
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_prev_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node first;
    struct aws_linked_list_node second;

    aws_linked_list_init(&list);
    aws_linked_list_push_back(&list, &first);
    aws_linked_list_push_back(&list, &second);

    struct aws_linked_list_node *result = aws_linked_list_prev(&second);

    assert(result == &first);
    assert(second.prev == &first);
    assert(first.next == &second);
    assert(aws_linked_list_node_prev_is_valid(&second));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    ensure_valid_linked_list(&list);

    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;

    aws_linked_list_insert_after(&list.head, &node);

    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == old_first);
    assert(old_first->prev == &node);
    assert(list.tail.prev == old_last || old_last == &list.head);
    if (old_last == &list.head) {
        assert(list.tail.prev == &node);
    }
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_next_is_valid(&list.head));
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    ensure_valid_linked_list(&list);

    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;

    aws_linked_list_insert_before(&list.tail, &node);

    assert(list.tail.prev == &node);
    assert(node.next == &list.tail);
    assert(node.prev == old_last);
    assert(old_last->next == &node);
    assert(list.head.next == old_first || old_first == &list.tail);
    if (old_first == &list.tail) {
        assert(list.head.next == &node);
    }
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_prev_is_valid(&list.tail));
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_swap_nodes_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node first;
    struct aws_linked_list_node second;

    aws_linked_list_init(&list);
    aws_linked_list_push_back(&list, &first);
    aws_linked_list_push_back(&list, &second);

    aws_linked_list_swap_nodes(&first, &second);

    assert(list.head.next == &second);
    assert(second.prev == &list.head);
    assert(second.next == &first);
    assert(first.prev == &second);
    assert(first.next == &list.tail);
    assert(list.tail.prev == &first);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_prev_is_valid(&first));
    assert(aws_linked_list_node_next_is_valid(&first));
    assert(aws_linked_list_node_prev_is_valid(&second));
    assert(aws_linked_list_node_next_is_valid(&second));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_remove_harness() {
    struct aws_linked_list list;
    ensure_valid_linked_list(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *node = list.head.next;
    struct aws_linked_list_node *old_next = node->next;

    aws_linked_list_remove(node);

    assert(node->next == NULL);
    assert(node->prev == NULL);
    assert(list.head.next == old_next);
    assert(old_next->prev == &list.head);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(!aws_linked_list_node_is_in_list(node));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    ensure_valid_linked_list(&list);

    bool was_empty = aws_linked_list_empty(&list);
    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;

    aws_linked_list_push_back(&list, &node);

    assert(list.tail.prev == &node);
    assert(node.prev == old_last);
    assert(node.next == &list.tail);
    assert(old_last->next == &node);
    if (was_empty) {
        assert(list.head.next == &node);
    } else {
        assert(list.head.next == old_first);
        assert(old_first->prev == &list.head);
    }
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_back_harness() {
    struct aws_linked_list list;
    ensure_valid_linked_list(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;

    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    assert(result == old_last);
    assert(result == list.tail.prev);
    assert(list.head.next == old_first);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    ensure_valid_linked_list(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_back = list.tail.prev;
    struct aws_linked_list_node *old_before_back = old_back->prev;

    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    assert(result == old_back);
    assert(result->next == NULL);
    assert(result->prev == NULL);
    assert(list.tail.prev == old_before_back);
    assert(old_before_back->next == &list.tail);
    if (old_first == old_back) {
        assert(list.head.next == &list.tail);
        assert(aws_linked_list_empty(&list));
    } else {
        assert(list.head.next == old_first);
        assert(old_first->prev == &list.head);
    }
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(!aws_linked_list_node_is_in_list(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    ensure_valid_linked_list(&list);

    bool was_empty = aws_linked_list_empty(&list);
    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;

    aws_linked_list_push_front(&list, &node);

    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == old_first);
    assert(old_first->prev == &node);
    if (was_empty) {
        assert(list.tail.prev == &node);
    } else {
        assert(list.tail.prev == old_last);
        assert(old_last->next == &list.tail);
    }
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_front_harness() {
    struct aws_linked_list list;
    ensure_valid_linked_list(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;

    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    assert(result == old_first);
    assert(result == list.head.next);
    assert(list.tail.prev == old_last);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    ensure_valid_linked_list(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_after_front = old_front->next;
    struct aws_linked_list_node *old_last = list.tail.prev;

    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    assert(result == old_front);
    assert(result->next == NULL);
    assert(result->prev == NULL);
    assert(list.head.next == old_after_front);
    assert(old_after_front->prev == &list.head);
    if (old_last == old_front) {
        assert(list.tail.prev == &list.head);
        assert(aws_linked_list_empty(&list));
    } else {
        assert(list.tail.prev == old_last);
        assert(old_last->next == &list.tail);
    }
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(!aws_linked_list_node_is_in_list(result));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    struct aws_linked_list b;
    ensure_valid_linked_list(&a);
    ensure_valid_linked_list(&b);

    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);
    struct aws_linked_list_node *a_old_first = a.head.next;
    struct aws_linked_list_node *a_old_last = a.tail.prev;
    struct aws_linked_list_node *b_old_first = b.head.next;
    struct aws_linked_list_node *b_old_last = b.tail.prev;

    aws_linked_list_swap_contents(&a, &b);

    if (b_was_empty) {
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
        assert(aws_linked_list_empty(&a));
    } else {
        assert(a.head.next == b_old_first);
        assert(a.tail.prev == b_old_last);
        assert(b_old_first->prev == &a.head);
        assert(b_old_last->next == &a.tail);
    }

    if (a_was_empty) {
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
        assert(aws_linked_list_empty(&b));
    } else {
        assert(b.head.next == a_old_first);
        assert(b.tail.prev == a_old_last);
        assert(a_old_first->prev == &b.head);
        assert(a_old_last->next == &b.tail);
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
    ensure_valid_linked_list(&dst);
    ensure_valid_linked_list(&src);

    bool src_was_empty = aws_linked_list_empty(&src);
    struct aws_linked_list_node *dst_old_front = dst.head.next;
    struct aws_linked_list_node *dst_old_back = dst.tail.prev;
    struct aws_linked_list_node *src_old_front = src.head.next;
    struct aws_linked_list_node *src_old_back = src.tail.prev;

    aws_linked_list_move_all_back(&dst, &src);

    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
    assert(aws_linked_list_empty(&src));

    if (src_was_empty) {
        assert(dst.head.next == dst_old_front);
        assert(dst.tail.prev == dst_old_back);
    } else {
        assert(dst_old_back->next == src_old_front);
        assert(src_old_front->prev == dst_old_back);
        assert(dst.tail.prev == src_old_back);
        assert(src_old_back->next == &dst.tail);
        assert(dst.head.next == dst_old_front);
    }

    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));
}

void aws_linked_list_move_all_front_harness() {
    struct aws_linked_list dst;
    struct aws_linked_list src;
    ensure_valid_linked_list(&dst);
    ensure_valid_linked_list(&src);

    bool src_was_empty = aws_linked_list_empty(&src);
    struct aws_linked_list_node *dst_old_front = dst.head.next;
    struct aws_linked_list_node *dst_old_back = dst.tail.prev;
    struct aws_linked_list_node *src_old_front = src.head.next;
    struct aws_linked_list_node *src_old_back = src.tail.prev;

    aws_linked_list_move_all_front(&dst, &src);

    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
    assert(aws_linked_list_empty(&src));

    if (src_was_empty) {
        assert(dst.head.next == dst_old_front);
        assert(dst.tail.prev == dst_old_back);
    } else {
        assert(dst.head.next == src_old_front);
        assert(src_old_front->prev == &dst.head);
        assert(src_old_back->next == dst_old_front);
        assert(dst_old_front->prev == src_old_back);
        assert(dst.tail.prev == dst_old_back);
    }

    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));
}

void aws_linked_list_node_is_in_list_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    aws_linked_list_init(&list);
    aws_linked_list_push_back(&list, &node);

    bool result = aws_linked_list_node_is_in_list(&node);

    assert(result);
    assert(result == (aws_linked_list_node_prev_is_valid(&node) && aws_linked_list_node_next_is_valid(&node)));
    assert(node.prev == &list.head);
    assert(node.next == &list.tail);
    assert(aws_linked_list_is_valid(&list));
}
