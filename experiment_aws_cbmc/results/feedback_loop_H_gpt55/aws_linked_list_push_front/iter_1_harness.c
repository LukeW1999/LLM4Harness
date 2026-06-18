#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static void make_empty_linked_list(struct aws_linked_list *list) {
    ensure_linked_list_is_allocated(list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(list);
    assert(aws_linked_list_is_valid(list));
    assert(aws_linked_list_empty(list));
}

void aws_linked_list_node_reset_harness(void) {
    struct aws_linked_list_node node;

    aws_linked_list_node_reset(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(!aws_linked_list_node_is_in_list(&node));
}

void aws_linked_list_empty_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    bool add_node = nondet_bool();

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&node);

    if (add_node) {
        aws_linked_list_push_back(&list, &node);
    }

    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    bool result = aws_linked_list_empty(&list);

    if (add_node) {
        assert(!result);
    } else {
        assert(result);
    }

    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_is_valid_harness(void) {
    struct aws_linked_list list;
    bool valid_case = nondet_bool();

    if (valid_case) {
        make_empty_linked_list(&list);
    } else {
        list.head.next = NULL;
        list.head.prev = NULL;
        list.tail.next = NULL;
        list.tail.prev = NULL;
    }

    struct aws_linked_list old = list;

    bool result = aws_linked_list_is_valid(&list);

    if (valid_case) {
        assert(result);
    } else {
        assert(!result);
    }

    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
}

void aws_linked_list_is_valid_deep_harness(void) {
    struct aws_linked_list list;
    bool valid_case = nondet_bool();

    if (valid_case) {
        make_empty_linked_list(&list);
    } else {
        list.head.next = NULL;
        list.head.prev = NULL;
        list.tail.next = NULL;
        list.tail.prev = NULL;
    }

    struct aws_linked_list old = list;

    bool result = aws_linked_list_is_valid_deep(&list);

    if (valid_case) {
        assert(result);
    } else {
        assert(!result);
    }

    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
}

void aws_linked_list_init_harness(void) {
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    aws_linked_list_init(&list);

    assert(list.head.next == &list.tail);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_empty(&list));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_begin_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    bool add_node = nondet_bool();

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&node);

    if (add_node) {
        aws_linked_list_push_back(&list, &node);
    }

    struct aws_linked_list old = list;

    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    assert(result == old.head.next);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_end_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    bool add_node = nondet_bool();

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&node);

    if (add_node) {
        aws_linked_list_push_back(&list, &node);
    }

    struct aws_linked_list old = list;

    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    assert(result == &list.tail);
    assert(result == &old.tail);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_rbegin_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    bool add_node = nondet_bool();

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&node);

    if (add_node) {
        aws_linked_list_push_back(&list, &node);
    }

    struct aws_linked_list old = list;

    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    assert(result == old.tail.prev);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_rend_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    bool add_node = nondet_bool();

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&node);

    if (add_node) {
        aws_linked_list_push_back(&list, &node);
    }

    struct aws_linked_list old = list;

    const struct aws_linked_list_node *result = aws_linked_list_rend(&list);

    assert(result == &list.head);
    assert(result == &old.head);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_next_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&node);
    aws_linked_list_push_back(&list, &node);

    struct aws_linked_list old = list;
    struct aws_linked_list_node old_node = node;

    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    assert(result == &list.tail);
    assert(result == old_node.next);
    assert(node.next == old_node.next);
    assert(node.prev == old_node.prev);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_prev_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&node);
    aws_linked_list_push_back(&list, &node);

    struct aws_linked_list old = list;
    struct aws_linked_list_node old_node = node;

    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    assert(result == &list.head);
    assert(result == old_node.prev);
    assert(node.next == old_node.next);
    assert(node.prev == old_node.prev);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_insert_after_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node existing;
    struct aws_linked_list_node to_add;
    bool has_existing = nondet_bool();

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&existing);
    aws_linked_list_node_reset(&to_add);

    if (has_existing) {
        aws_linked_list_push_back(&list, &existing);
    }

    struct aws_linked_list_node *after = has_existing ? &existing : &list.head;
    struct aws_linked_list_node *old_next = after->next;

    aws_linked_list_insert_after(after, &to_add);

    assert(after->next == &to_add);
    assert(to_add.prev == after);
    assert(to_add.next == old_next);
    assert(old_next->prev == &to_add);
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_insert_before_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node existing;
    struct aws_linked_list_node to_add;
    bool has_existing = nondet_bool();

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&existing);
    aws_linked_list_node_reset(&to_add);

    if (has_existing) {
        aws_linked_list_push_back(&list, &existing);
    }

    struct aws_linked_list_node *before = has_existing ? &existing : &list.tail;
    struct aws_linked_list_node *old_prev = before->prev;

    aws_linked_list_insert_before(before, &to_add);

    assert(before->prev == &to_add);
    assert(to_add.next == before);
    assert(to_add.prev == old_prev);
    assert(old_prev->next == &to_add);
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_swap_nodes_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node a;
    struct aws_linked_list_node b;
    bool same_node = nondet_bool();

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&a);
    aws_linked_list_node_reset(&b);
    aws_linked_list_push_back(&list, &a);
    aws_linked_list_push_back(&list, &b);

    struct aws_linked_list old = list;
    struct aws_linked_list_node old_a = a;
    struct aws_linked_list_node old_b = b;

    aws_linked_list_swap_nodes(&a, same_node ? &a : &b);

    if (same_node) {
        assert(a.next == old_a.next);
        assert(a.prev == old_a.prev);
        assert(b.next == old_b.next);
        assert(b.prev == old_b.prev);
        assert(list.head.next == old.head.next);
        assert(list.tail.prev == old.tail.prev);
    } else {
        assert(list.head.next == &b);
        assert(b.prev == &list.head);
        assert(b.next == &a);
        assert(a.prev == &b);
        assert(a.next == &list.tail);
        assert(list.tail.prev == &a);
    }

    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_is_in_list(&a));
    assert(aws_linked_list_node_is_in_list(&b));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_remove_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node before;
    struct aws_linked_list_node node;
    struct aws_linked_list_node after;
    bool has_before = nondet_bool();
    bool has_after = nondet_bool();

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&before);
    aws_linked_list_node_reset(&node);
    aws_linked_list_node_reset(&after);

    if (has_before) {
        aws_linked_list_push_back(&list, &before);
    }
    aws_linked_list_push_back(&list, &node);
    if (has_after) {
        aws_linked_list_push_back(&list, &after);
    }

    struct aws_linked_list_node *old_prev = node.prev;
    struct aws_linked_list_node *old_next = node.next;

    aws_linked_list_remove(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);
    assert(!aws_linked_list_node_is_in_list(&node));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_push_back_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node old_node;
    struct aws_linked_list_node node;
    bool was_nonempty = nondet_bool();

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&old_node);
    aws_linked_list_node_reset(&node);

    if (was_nonempty) {
        aws_linked_list_push_back(&list, &old_node);
    }

    struct aws_linked_list_node *old_last = list.tail.prev;
    struct aws_linked_list_node *old_first = list.head.next;

    aws_linked_list_push_back(&list, &node);

    assert(list.tail.prev == &node);
    assert(node.prev == old_last);
    assert(node.next == &list.tail);
    assert(old_last->next == &node);
    assert(list.head.next == old_first);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_is_in_list(&node));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_back_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node old_node;
    struct aws_linked_list_node node;

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&old_node);
    aws_linked_list_node_reset(&node);
    aws_linked_list_push_back(&list, &old_node);
    aws_linked_list_push_back(&list, &node);

    struct aws_linked_list old = list;
    struct aws_linked_list_node old_node_state = node;

    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    assert(result == &node);
    assert(result == old.tail.prev);
    assert(node.next == old_node_state.next);
    assert(node.prev == old_node_state.prev);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_pop_back_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node old_node;
    struct aws_linked_list_node node;

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&old_node);
    aws_linked_list_node_reset(&node);
    aws_linked_list_push_back(&list, &old_node);
    aws_linked_list_push_back(&list, &node);

    struct aws_linked_list_node *new_last = node.prev;

    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    assert(result == &node);
    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(list.tail.prev == new_last);
    assert(new_last->next == &list.tail);
    assert(!aws_linked_list_node_is_in_list(&node));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_push_front_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node old_node;
    struct aws_linked_list_node node;
    bool was_nonempty = nondet_bool();

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&old_node);
    aws_linked_list_node_reset(&node);

    if (was_nonempty) {
        aws_linked_list_push_back(&list, &old_node);
    }

    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;

    aws_linked_list_push_front(&list, &node);

    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == old_first);
    assert(old_first->prev == &node);
    assert(list.tail.prev == old_last);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_is_in_list(&node));
    assert(aws_linked_list_is_valid_deep(&list));
}

void aws_linked_list_front_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    struct aws_linked_list_node old_node;

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&node);
    aws_linked_list_node_reset(&old_node);
    aws_linked_list_push_back(&list, &node);
    aws_linked_list_push_back(&list, &old_node);

    struct aws_linked_list old = list;
    struct aws_linked_list_node old_node_state = node;

    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    assert(result == &node);
    assert(result == old.head.next);
    assert(node.next == old_node_state.next);
    assert(node.prev == old_node_state.prev);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
    assert(aws_linked_list_is_valid_deep(&list));
}
