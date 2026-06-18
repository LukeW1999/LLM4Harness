#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&a));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&b));

    struct aws_linked_list_node *a_first = a.head.next;
    struct aws_linked_list_node *a_last = a.tail.prev;
    struct aws_linked_list_node *b_first = b.head.next;
    struct aws_linked_list_node *b_last = b.tail.prev;

    struct aws_linked_list_node *a_head_prev = a.head.prev;
    struct aws_linked_list_node *a_tail_next = a.tail.next;
    struct aws_linked_list_node *b_head_prev = b.head.prev;
    struct aws_linked_list_node *b_tail_next = b.tail.next;

    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);

    aws_linked_list_swap_contents(&a, &b);

    assert(aws_linked_list_empty(&a) == b_was_empty);
    assert(aws_linked_list_empty(&b) == a_was_empty);

    if (b_was_empty) {
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        assert(a.head.next == b_first);
        assert(a.tail.prev == b_last);
        assert(b_first->prev == &a.head);
        assert(b_last->next == &a.tail);
    }

    if (a_was_empty) {
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        assert(b.head.next == a_first);
        assert(b.tail.prev == a_last);
        assert(a_first->prev == &b.head);
        assert(a_last->next == &b.tail);
    }

    assert(a.head.prev == a_head_prev);
    assert(a.tail.next == a_tail_next);
    assert(b.head.prev == b_head_prev);
    assert(b.tail.next == b_tail_next);

    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));
}
