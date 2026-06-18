#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_swap_contents_harness() {
    /* Allocate and bound the two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Save old state */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    struct aws_linked_list_node *old_a_first = a.head.next;
    struct aws_linked_list_node *old_a_last  = a.tail.prev;
    bool old_a_was_empty = (old_a_first == &a.tail);
    bool old_b_was_empty = aws_linked_list_empty(&b);

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* Postconditions for list a */
    if (old_b_was_empty) {
        /* a should become empty */
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* a should now contain b's original contents */
        assert(a.head.next == old_b.head.next);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev == old_b.tail.prev);
        assert(a.tail.prev->next == &a.tail);
    }

    /* Postconditions for list b */
    if (old_a_was_empty) {
        /* b should become empty */
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* b should now contain a's original contents */
        assert(b.head.next == old_a_first);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev == old_a_last);
        assert(b.tail.prev->next == &b.tail);
    }

    /* Unchanged fields for list a */
    assert(a.head.prev == old_a.head.prev);
    assert(a.tail.next == old_a.tail.next);

    /* Unchanged fields for list b */
    assert(b.head.prev == old_b.head.prev);
    assert(b.tail.next == old_b.tail.next);

    /* Validity invariants */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
}
