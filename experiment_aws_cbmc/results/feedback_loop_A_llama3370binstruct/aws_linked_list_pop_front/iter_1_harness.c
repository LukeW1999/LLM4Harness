#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_init_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old = list;

    aws_linked_list_init(&list);

    assert(list.head.next == &list.tail);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_empty(&list));

    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);
}

void aws_linked_list_empty_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    bool result = aws_linked_list_empty(&list);

    assert(result == (list.head.next == &list.tail));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_is_valid_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    bool result = aws_linked_list_is_valid(&list);

    assert(result == (list.head.next && list.head.prev == NULL && list.tail.prev && list.tail.next == NULL));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_is_valid_deep_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    bool result = aws_linked_list_is_valid_deep(&list);

    assert(result == (list.head.next == &list.tail || (list.head.next->prev == &list.head && list.tail.prev->next == &list.tail)));
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    __CPROVER_assume(node.next != NULL);
    __CPROVER_assume(node.prev != NULL);

    struct aws_linked_list_node old = node;

    aws_linked_list_node_reset(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(aws_linked_list_node_is_in_list(&node) == false);
}

void aws_linked_list_begin_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    assert(result == list.head.next);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_end_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    assert(result == &list.tail);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_rbegin_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    assert(result == list.tail.prev);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_rend_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    const struct aws_linked_list_node *result = aws_linked_list_rend(&list);

    assert(result == &list.head);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_next_harness() {
    struct aws_linked_list_node node;
    __CPROVER_assume(node.next != NULL);
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node));

    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    assert(result == node.next);
    assert(aws_linked_list_node_next_is_valid(&node));
}

void aws_linked_list_prev_harness() {
    struct aws_linked_list_node node;
    __CPROVER_assume(node.prev != NULL);
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&node));

    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    assert(result == node.prev);
    assert(aws_linked_list_node_prev_is_valid(&node));
}

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list_node after;
    struct aws_linked_list_node to_add;
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&after));
    __CPROVER_assume(to_add.next == NULL);
    __CPROVER_assume(to_add.prev == NULL);

    struct aws_linked_list_node old_after = after;
    struct aws_linked_list_node old_to_add = to_add;

    aws_linked_list_insert_after(&after, &to_add);

    assert(after.next->prev == &after);
    assert(to_add.prev == &after);
    assert(to_add.next == old_after.next);
    assert(old_after.next->prev == &to_add);
    assert(aws_linked_list_node_next_is_valid(&after));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));
}

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list_node before;
    struct aws_linked_list_node to_add;
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&before));
    __CPROVER_assume(to_add.next == NULL);
    __CPROVER_assume(to_add.prev == NULL);

    struct aws_linked_list_node old_before = before;
    struct aws_linked_list_node old_to_add = to_add;

    aws_linked_list_insert_before(&before, &to_add);

    assert(before.prev->next == &before);
    assert(to_add.next == &before);
    assert(to_add.prev == old_before.prev);
    assert(old_before.prev->next == &to_add);
    assert(aws_linked_list_node_prev_is_valid(&before));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));
}

void aws_linked_list_remove_harness() {
    struct aws_linked_list_node node;
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&node));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node));

    struct aws_linked_list_node old_node = node;

    aws_linked_list_remove(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
    assert(old_node.prev->next == old_node.next);
    assert(old_node.next->prev == old_node.prev);
}

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    struct aws_linked_list_node node;
    __CPROVER_assume(node.next == NULL);
    __CPROVER_assume(node.prev == NULL);

    struct aws_linked_list old = list;

    aws_linked_list_push_back(&list, &node);

    assert(list.tail.prev == &node);
    assert(node.prev == old.tail.prev);
    assert(node.next == &list.tail);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    struct aws_linked_list_node node;
    __CPROVER_assume(node.next == NULL);
    __CPROVER_assume(node.prev == NULL);

    struct aws_linked_list old = list;

    aws_linked_list_push_front(&list, &node);

    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == old.head.next);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    assert(result->next == NULL);
    assert(result->prev == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    assert(result->next == NULL);
    assert(result->prev == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    struct aws_linked_list b;
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    aws_linked_list_swap_contents(&a, &b);

    assert(a.head.next == old_b.head.next);
    assert(a.head.prev == old_b.head.prev);
    assert(a.tail.next == old_b.tail.next);
    assert(a.tail.prev == old_b.tail.prev);
    assert(b.head.next == old_a.head.next);
    assert(b.head.prev == old_a.head.prev);
    assert(b.tail.next == old_a.tail.next);
    assert(b.tail.prev == old_a.tail.prev);
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
}

void aws_linked_list_move_all_back_harness() {
    struct aws_linked_list dst;
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    struct aws_linked_list src;
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    aws_linked_list_move_all_back(&dst, &src);

    assert(dst.head.next == old_src.head.next);
    assert(dst.head.prev == old_dst.head.prev);
    assert(dst.tail.next == old_dst.tail.next);
    assert(dst.tail.prev == old_src.tail.prev);
    assert(src.head.next == &src.tail);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
    assert(src.tail.prev == &src.head);
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));
}

void aws_linked_list_move_all_front_harness() {
    struct aws_linked_list dst;
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    struct aws_linked_list src;
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    aws_linked_list_move_all_front(&dst, &src);

    assert(dst.head.next == old_src.head.next);
    assert(dst.head.prev == old_dst.head.prev);
    assert(dst.tail.next == old_dst.tail.next);
    assert(dst.tail.prev == old_src.tail.prev);
    assert(src.head.next == &src.tail);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
    assert(src.tail.prev == &src.head);
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));
}

void aws_linked_list_node_is_in_list_harness() {
    struct aws_linked_list_node node;
    __CPROVER_assume(node.next != NULL);
    __CPROVER_assume(node.prev != NULL);

    bool result = aws_linked_list_node_is_in_list(&node);

    assert(result == (aws_linked_list_node_prev_is_valid(&node) && aws_linked_list_node_next_is_valid(&node)));
}
