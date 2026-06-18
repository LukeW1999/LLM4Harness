#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_swap_contents_harness() {
    /* Declare two non-aliasing lists */
    struct aws_linked_list a, b;

    /* Allocate nodes for each list up to MAX_LINKED_LIST_ITEM_ALLOCATION */
    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume both lists are valid */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Save old state */
    bool old_a_empty = aws_linked_list_empty(&a);
    bool old_b_empty = aws_linked_list_empty(&b);

    struct aws_linked_list_node *old_a_head_next = a.head.next;
    struct aws_linked_list_node *old_a_tail_prev = a.tail.prev;

    struct aws_linked_list_node *old_b_head_next = b.head.next;
    struct aws_linked_list_node *old_b_tail_prev = b.tail.prev;

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* Postconditions: validity */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* Postconditions: contents swapped */
    if (old_a_empty && old_b_empty) {
        /* Both were empty, both should still be empty */
        assert(aws_linked_list_empty(&a));
        assert(aws_linked_list_empty(&b));
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else if (old_a_empty && !old_b_empty) {
        /* a was empty, b had nodes. Now a contains b's old nodes, b is empty */
        assert(!aws_linked_list_empty(&a));
        assert(aws_linked_list_empty(&b));
        assert(a.head.next == old_b_head_next);
        assert(a.tail.prev == old_b_tail_prev);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev->next == &a.tail);
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else if (!old_a_empty && old_b_empty) {
        /* b was empty, a had nodes. Now b contains a's old nodes, a is empty */
        assert(aws_linked_list_empty(&a));
        assert(!aws_linked_list_empty(&b));
        assert(b.head.next == old_a_head_next);
        assert(b.tail.prev == old_a_tail_prev);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev->next == &b.tail);
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* Both had nodes. Nodes are swapped */
        assert(!aws_linked_list_empty(&a));
        assert(!aws_linked_list_empty(&b));
        assert(a.head.next == old_b_head_next);
        assert(a.tail.prev == old_b_tail_prev);
        assert(b.head.next == old_a_head_next);
        assert(b.tail.prev == old_a_tail_prev);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev->next == &a.tail);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev->next == &b.tail);
    }
}
