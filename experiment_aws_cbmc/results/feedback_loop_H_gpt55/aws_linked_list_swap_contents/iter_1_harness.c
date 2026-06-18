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
    __CPROVER_assume(aws_linked_list_is_valid_deep(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&b));

    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&a, sizeof(a)));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&b, sizeof(b)));

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

    if (!old_a_empty) {
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(old_a_first, sizeof(*old_a_first)));
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(old_a_last, sizeof(*old_a_last)));
    }

    if (!old_b_empty) {
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(old_b_first, sizeof(*old_b_first)));
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(old_b_last, sizeof(*old_b_last)));
    }

    aws_linked_list_swap_contents(&a, &b);

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
        assert(aws_linked_list_begin(&a) == old_b_first);
        assert(aws_linked_list_rbegin(&a) == old_b_last);
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
        assert(aws_linked_list_begin(&b) == old_a_first);
        assert(aws_linked_list_rbegin(&b) == old_a_last);
    }

    assert(a.head.prev == old_a_head_prev);
    assert(a.tail.next == old_a_tail_next);
    assert(b.head.prev == old_b_head_prev);
    assert(b.tail.next == old_b_tail_next);

    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid(&b));
    assert(aws_linked_list_is_valid_deep(&b));
}
