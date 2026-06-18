#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static void ensure_valid_bounded_linked_list(struct aws_linked_list *list) {
    ensure_linked_list_is_allocated(list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(list));
}

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    aws_linked_list_node_reset(&node);
    assert(node.next == NULL);
    assert(node.prev == NULL);
}

void aws_linked_list_empty_harness() {
    struct aws_linked_list list;
    ensure_valid_bounded_linked_list(&list);

    struct aws_linked_list old = list;

    bool result = aws_linked_list_empty(&list);

    assert(result == (old.head.next == &list.tail));
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
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
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_begin_harness() {
    struct aws_linked_list list;
    ensure_valid_bounded_linked_list(&list);

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
    ensure_valid_bounded_linked_list(&list);

    struct aws_linked_list old = list;

    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    assert(result == &list.tail);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_rbegin_harness() {
    struct aws_linked_list list;
    ensure_valid_bounded_linked_list(&list);

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
    ensure_valid_bounded_linked_list(&list);

    struct aws_linked_list old = list;

    const struct aws_linked_list_node *result = aws_linked_list_rend(&list);

    assert(result == &list.head);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_next_harness() {
    struct aws_linked_list list;
    ensure_valid_bounded_linked_list(&list);

    struct aws_linked_list_node *node = &list.head;
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list old = list;

    struct aws_linked_list_node *result = aws_linked_list_next(node);

    assert(result == old_next);
    assert(node->next == old_next);
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_prev_harness() {
    struct aws_linked_list list;
    ensure_valid_bounded_linked_list(&list);

    struct aws_linked_list_node *node = &list.tail;
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list old = list;

    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    assert(result == old_prev);
    assert(node->prev == old_prev);
    assert(aws_linked_list_node_prev_is_valid(node));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    ensure_valid_bounded_linked_list(&list);
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

    ensure_valid_bounded_linked_list(&list);
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

void aws_linked_list_remove_harness() {
    struct aws_linked_list list;
    ensure_valid_bounded_linked_list(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *node = list.head.next;
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    aws_linked_list_remove(node);

    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);
    assert(node->next == NULL);
    assert(node->prev == NULL);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    ensure_valid_bounded_linked_list(&list);
    aws_linked_list_node_reset(&node);

    struct aws_linked_list_node *old_last = list.tail.prev;

    aws_linked_list_push_back(&list, &node);

    assert(list.tail.prev == &node);
    assert(node.prev == old_last);
    assert(node.next == &list.tail);
    assert(old_last->next == &node);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_back_harness() {
    struct aws_linked_list list;
    ensure_valid_bounded_linked_list(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list old = list;

    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    assert(result == old.tail.prev);
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    ensure_valid_bounded_linked_list(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_back = list.tail.prev;
    struct aws_linked_list_node *old_back_prev = old_back->prev;

    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    assert(result == old_back);
    assert(result->next == NULL);
    assert(result->prev == NULL);
    assert(old_back_prev->next == &list.tail);
    assert(list.tail.prev == old_back_prev);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    ensure_valid_bounded_linked_list(&list);
    aws_linked_list_node_reset(&node);

    struct aws_linked_list_node *old_first = list.head.next;

    aws_linked_list_push_front(&list, &node);

    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == old_first);
    assert(old_first->prev == &node);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_front_harness() {
    struct aws_linked_list list;
    ensure_valid_bounded_linked_list(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list old = list;

    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    assert(result == old.head.next);
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    ensure_valid_bounded_linked_list(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_front_next = old_front->next;

    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    assert(result == old_front);
    assert(result->next == NULL);
    assert(result->prev == NULL);
    assert(list.head.next == old_front_next);
    assert(old_front_next->prev == &list.head);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_swap_nodes_harness() {
    struct aws_linked_list list;
    ensure_valid_bounded_linked_list(&list);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *a = list.head.next;
    struct aws_linked_list_node *b = list.tail.prev;

    struct aws_linked_list_node *old_a_prev = a->prev;
    struct aws_linked_list_node *old_a_next = a->next;
    struct aws_linked_list_node *old_b_prev = b->prev;
    struct aws_linked_list_node *old_b_next = b->next;

    aws_linked_list_swap_nodes(a, b);

    if (a == b) {
        assert(a->prev == old_a_prev);
        assert(a->next == old_a_next);
    } else if (old_a_next == b) {
        assert(b->prev == old_a_prev);
        assert(b->next == a);
        assert(a->prev == b);
        assert(a->next == old_b_next);
        assert(old_a_prev->next == b);
        assert(old_b_next->prev == a);
    } else if (old_b_next == a) {
        assert(a->prev == old_b_prev);
        assert(a->next == b);
        assert(b->prev == a);
        assert(b->next == old_a_next);
        assert(old_b_prev->next == a);
        assert(old_a_next->prev == b);
    } else {
        assert(a->prev == old_b_prev);
        assert(a->next == old_b_next);
        assert(b->prev == old_a_prev);
        assert(b->next == old_a_next);
        assert(old_b_prev->next == a);
        assert(old_b_next->prev == a);
        assert(old_a_prev->next == b);
        assert(old_a_next->prev == b);
    }

    assert(aws_linked_list_node_prev_is_valid(a));
    assert(aws_linked_list_node_next_is_valid(a));
    assert(aws_linked_list_node_prev_is_valid(b));
    assert(aws_linked_list_node_next_is_valid(b));
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_valid_bounded_linked_list(&a);
    ensure_valid_bounded_linked_list(&b);

    struct aws_linked_list_node *old_a_first = a.head.next;
    struct aws_linked_list_node *old_a_last = a.tail.prev;
    struct aws_linked_list_node *old_b_first = b.head.next;
    struct aws_linked_list_node *old_b_last = b.tail.prev;
    bool old_a_empty = aws_linked_list_empty(&a);
    bool old_b_empty = aws_linked_list_empty(&b);

    aws_linked_list_swap_contents(&a, &b);

    if (old_b_empty) {
        assert(aws_linked_list_empty(&a));
    }
