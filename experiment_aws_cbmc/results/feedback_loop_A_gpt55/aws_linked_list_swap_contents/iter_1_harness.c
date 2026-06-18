#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

static size_t save_linked_list_nodes(
    struct aws_linked_list *list,
    struct aws_linked_list_node **nodes) {
    size_t len = 0;
    struct aws_linked_list_node *cur = list->head.next;

    while (cur != &list->tail && len < MAX_LINKED_LIST_ITEM_ALLOCATION) {
        nodes[len] = cur;
        cur = cur->next;
        ++len;
    }

    __CPROVER_assume(cur == &list->tail);
    return len;
}

static void assert_linked_list_matches_saved_nodes(
    const struct aws_linked_list *list,
    struct aws_linked_list_node **nodes,
    size_t len) {
    size_t i = 0;
    const struct aws_linked_list_node *cur = list->head.next;

    while (cur != &list->tail && i < MAX_LINKED_LIST_ITEM_ALLOCATION) {
        if (i < len) {
            assert(cur == nodes[i]);
        } else {
            assert(false);
            break;
        }

        cur = cur->next;
        ++i;
    }

    assert(cur == &list->tail);
    assert(i == len);
}

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&a));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&b));

    struct aws_linked_list_node *a_nodes[MAX_LINKED_LIST_ITEM_ALLOCATION + 1];
    struct aws_linked_list_node *b_nodes[MAX_LINKED_LIST_ITEM_ALLOCATION + 1];

    size_t a_len = save_linked_list_nodes(&a, a_nodes);
    size_t b_len = save_linked_list_nodes(&b, b_nodes);

    bool old_a_empty = aws_linked_list_empty(&a);
    bool old_b_empty = aws_linked_list_empty(&b);

    struct aws_linked_list_node *old_a_first = a.head.next;
    struct aws_linked_list_node *old_a_last = a.tail.prev;
    struct aws_linked_list_node *old_b_first = b.head.next;
    struct aws_linked_list_node *old_b_last = b.tail.prev;

    struct aws_linked_list_node *old_a_head_prev = a.head.prev;
    struct aws_linked_list_node *old_a_tail_next = a.tail.next;
    struct aws_linked_list_node *old_b_head_prev = b.head.prev;
    struct aws_linked_list_node *old_b_tail_next = b.tail.next;

    aws_linked_list_swap_contents(&a, &b);

    assert(a.head.prev == old_a_head_prev);
    assert(a.tail.next == old_a_tail_next);
    assert(b.head.prev == old_b_head_prev);
    assert(b.tail.next == old_b_tail_next);

    if (old_b_empty) {
        assert(aws_linked_list_empty(&a));
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        assert(!aws_linked_list_empty(&a));
        assert(a.head.next == old_b_first);
        assert(a.tail.prev == old_b_last);
        assert(old_b_first->prev == &a.head);
        assert(old_b_last->next == &a.tail);
    }

    if (old_a_empty) {
        assert(aws_linked_list_empty(&b));
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        assert(!aws_linked_list_empty(&b));
        assert(b.head.next == old_a_first);
        assert(b.tail.prev == old_a_last);
        assert(old_a_first->prev == &b.head);
        assert(old_a_last->next == &b.tail);
    }

    assert_linked_list_matches_saved_nodes(&a, b_nodes, b_len);
    assert_linked_list_matches_saved_nodes(&b, a_nodes, a_len);

    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));
}
