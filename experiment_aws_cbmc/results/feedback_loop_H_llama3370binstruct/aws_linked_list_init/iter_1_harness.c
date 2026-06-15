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

    aws_linked_list_init(&list);

    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_empty(&list));
    assert(list.head.next == &list.tail);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);
}

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    __CPROVER_assume(node.next!= NULL);
    __CPROVER_assume(node.prev!= NULL);

    aws_linked_list_node_reset(&node);

    assert(node.next == NULL);
    assert(node.prev == NULL);
}

void aws_linked_list_empty_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    bool result = aws_linked_list_empty(&list);

    assert(aws_linked_list_is_valid(&list));
    if (list.head.next == &list.tail) {
        assert(result == true);
    } else {
        assert(result == false);
    }
}

void aws_linked_list_is_valid_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    bool result = aws_linked_list_is_valid(&list);

    assert(aws_linked_list_is_valid(&list));
    assert(result == true);
}

void aws_linked_list_is_valid_deep_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    bool result = aws_linked_list_is_valid_deep(&list);

    assert(aws_linked_list_is_valid(&list));
    assert(result == true);
}

void aws_linked_list_node_next_is_valid_harness() {
    struct aws_linked_list_node node;
    __CPROVER_assume(node.next!= NULL);

    bool result = aws_linked_list_node_next_is_valid(&node);

    if (node.next->prev == &node) {
        assert(result == true);
    } else {
        assert(result == false);
    }
}

void aws_linked_list_node_prev_is_valid_harness() {
    struct aws_linked_list_node node;
    __CPROVER_assume(node.prev!= NULL);

    bool result = aws_linked_list_node_prev_is_valid(&node);

    if (node.prev->next == &node) {
        assert(result == true);
    } else {
        assert(result == false);
    }
}

void aws_linked_list_begin_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    assert(aws_linked_list_is_valid(&list));
    assert(result == list.head.next);
}

void aws_linked_list_end_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    assert(aws_linked_list_is_valid(&list));
    assert(result == &list.tail);
}

void aws_linked_list_rbegin_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    assert(aws_linked_list_is_valid(&list));
    assert(result == list.tail.prev);
}

void aws_linked_list_rend_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    const struct aws_linked_list_node *result = aws_linked_list_rend(&list);

    assert(aws_linked_list_is_valid(&list));
    assert(result == &list.head);
}

void aws_linked_list_next_harness() {
    struct aws_linked_list_node node;
    __CPROVER_assume(node.next!= NULL);
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node));

    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(result == node.next);
}

void aws_linked_list_prev_harness() {
    struct aws_linked_list_node node;
    __CPROVER_assume(node.prev!= NULL);
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&node));

    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(result == node.prev);
}

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list_node after;
    struct aws_linked_list_node to_add;
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&after));
    __CPROVER_assume(&to_add!= NULL);

    aws_linked_list_insert_after(&after, &to_add);

    assert(aws_linked_list_node_next_is_valid(&after));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(after.next == &to_add);
}

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list_node before;
    struct aws_linked_list_node to_add;
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&before));
    __CPROVER_assume(&to_add!= NULL);

    aws_linked_list_insert_before(&before, &to_add);

    assert(aws_linked_list_node_prev_is_valid(&before));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(before.prev == &to_add);
}

void aws_linked_list_remove_harness() {
    struct aws_linked_list_node node;
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&node));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node));

    aws_linked_list_remove(&node);

    assert(node.next == NULL && node.prev == NULL);
}

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    struct aws_linked_list_node node;
    __CPROVER_assume(&node!= NULL);

    aws_linked_list_push_back(&list, &node);

    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == &node);
}

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    struct aws_linked_list_node node;
    __CPROVER_assume(&node!= NULL);

    aws_linked_list_push_front(&list, &node);

    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next == &node);
}

void aws_linked_list_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(result == list.tail.prev);
}

void aws_linked_list_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));
    assert(result == list.head.next);
}

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    assert(result->next == NULL && result->prev == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    assert(result->next == NULL && result->prev == NULL);
    assert(aws_linked_list_is_valid(&list));
}

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    struct aws_linked_list b;
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&b));
    __CPROVER_assume(&a!= &b);

    aws_linked_list_swap_contents(&a, &b);

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
    __CPROVER_assume(&dst!= &src);

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));
}

void aws_linked_list_move_all_front_harness() {
    struct aws_linked_list dst;
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    struct aws_linked_list src;
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst!= &src);

    aws_linked_list_move_all_front(&dst, &src);

    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));
}

void aws_linked_list_node_is_in_list_harness() {
    struct aws_linked_list_node node;
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&node));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node));

    bool result = aws_linked_list_node_is_in_list(&node);

    assert(result == true);
}
