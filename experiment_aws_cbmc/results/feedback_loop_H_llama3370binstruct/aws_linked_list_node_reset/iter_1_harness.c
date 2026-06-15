#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    ensure_linked_list_is_allocated(&node, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_node_is_in_list(&node));

    struct aws_linked_list_node old_node = node;

    aws_linked_list_node_reset(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);

    assert(old_node.next!= node.next);
    assert(old_node.prev!= node.prev);
}

void aws_linked_list_empty_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    bool result = aws_linked_list_empty(&list);

    assert(result == (list.head.next == &list.tail));
}

void aws_linked_list_is_valid_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    bool result = aws_linked_list_is_valid(&list);

    assert(result == (list.head.next!= NULL && list.head.prev == NULL && list.tail.prev!= NULL && list.tail.next == NULL));
}

void aws_linked_list_node_next_is_valid_harness() {
    struct aws_linked_list_node node;
    ensure_linked_list_is_allocated(&node, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_node_is_in_list(&node));

    bool result = aws_linked_list_node_next_is_valid(&node);

    assert(result == (node.next!= NULL && node.next->prev == &node));
}

void aws_linked_list_node_prev_is_valid_harness() {
    struct aws_linked_list_node node;
    ensure_linked_list_is_allocated(&node, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_node_is_in_list(&node));

    bool result = aws_linked_list_node_prev_is_valid(&node);

    assert(result == (node.prev!= NULL && node.prev->next == &node));
}

void aws_linked_list_is_valid_deep_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    bool result = aws_linked_list_is_valid_deep(&list);

    assert(result == (list.head.next!= NULL && list.head.prev == NULL && list.tail.prev!= NULL && list.tail.next == NULL));
}

void aws_linked_list_init_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    aws_linked_list_init(&list);

    assert(list.head.next == &list.tail);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);
}

void aws_linked_list_begin_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    assert(result == list.head.next);
}

void aws_linked_list_end_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    assert(result == &list.tail);
}

void aws_linked_list_rbegin_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    assert(result == list.tail.prev);
}

void aws_linked_list_rend_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    const struct aws_linked_list_node *result = aws_linked_list_rend(&list);

    assert(result == &list.head);
}

void aws_linked_list_next_harness() {
    struct aws_linked_list_node node;
    ensure_linked_list_is_allocated(&node, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_node_is_in_list(&node));

    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    assert(result == node.next);
}

void aws_linked_list_prev_harness() {
    struct aws_linked_list_node node;
    ensure_linked_list_is_allocated(&node, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_node_is_in_list(&node));

    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    assert(result == node.prev);
}

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list_node after;
    ensure_linked_list_is_allocated(&after, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_node_is_in_list(&after));

    struct aws_linked_list_node to_add;
    ensure_linked_list_is_allocated(&to_add, MAX_LINKED_LIST_ITEM_ALLOCATION);

    aws_linked_list_insert_after(&after, &to_add);

    assert(after.next == &to_add);
    assert(to_add.prev == &after);
    assert(to_add.next == after.next);
}

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list_node before;
    ensure_linked_list_is_allocated(&before, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_node_is_in_list(&before));

    struct aws_linked_list_node to_add;
    ensure_linked_list_is_allocated(&to_add, MAX_LINKED_LIST_ITEM_ALLOCATION);

    aws_linked_list_insert_before(&before, &to_add);

    assert(before.prev == &to_add);
    assert(to_add.next == &before);
    assert(to_add.prev == before.prev);
}

void aws_linked_list_remove_harness() {
    struct aws_linked_list_node node;
    ensure_linked_list_is_allocated(&node, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_node_is_in_list(&node));

    aws_linked_list_remove(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
}

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    ensure_linked_list_is_allocated(&node, MAX_LINKED_LIST_ITEM_ALLOCATION);

    aws_linked_list_push_back(&list, &node);

    assert(list.tail.prev == &node);
    assert(node.prev == list.tail.prev);
}

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    ensure_linked_list_is_allocated(&node, MAX_LINKED_LIST_ITEM_ALLOCATION);

    aws_linked_list_push_front(&list, &node);

    assert(list.head.next == &node);
    assert(node.prev == &list.head);
}

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    assert(result->next == NULL);
    assert(result->prev == NULL);
}

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    assert(result->next == NULL);
    assert(result->prev == NULL);
}

void aws_linked_list_swap_nodes_harness() {
    struct aws_linked_list_node a;
    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_node_is_in_list(&a));

    struct aws_linked_list_node b;
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_node_is_in_list(&b));

    aws_linked_list_swap_nodes(&a, &b);

    assert(a.next == b.next);
    assert(a.prev == b.prev);
    assert(b.next == a.next);
    assert(b.prev == a.prev);
}

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&a));

    struct aws_linked_list b;
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    aws_linked_list_swap_contents(&a, &b);

    assert(a.head.next == b.head.next);
    assert(a.head.prev == b.head.prev);
    assert(a.tail.next == b.tail.next);
    assert(a.tail.prev == b.tail.prev);
}

void aws_linked_list_move_all_back_harness() {
    struct aws_linked_list dst;
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    struct aws_linked_list src;
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    aws_linked_list_move_all_back(&dst, &src);

    assert(src.head.next == &src.tail);
    assert(src.head.prev == NULL);
    assert(src.tail.prev == &src.head);
    assert(src.tail.next == NULL);
}

void aws_linked_list_move_all_front_harness() {
    struct aws_linked_list dst;
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    struct aws_linked_list src;
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    aws_linked_list_move_all_front(&dst, &src);

    assert(src.head.next == &src.tail);
    assert(src.head.prev == NULL);
    assert(src.tail.prev == &src.head);
    assert(src.tail.next == NULL);
}

void aws_linked_list_node_is_in_list_harness() {
    struct aws_linked_list_node node;
    ensure_linked_list_is_allocated(&node, MAX_LINKED_LIST_ITEM_ALLOCATION);

    bool result = aws_linked_list_node_is_in_list(&node);

    assert(result == (node.prev!= NULL && node.next!= NULL));
}
