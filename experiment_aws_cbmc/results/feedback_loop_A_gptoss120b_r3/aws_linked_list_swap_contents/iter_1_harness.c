#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_linked_list_swap_contents_harness() {
    /* Allocate and bound the two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Record whether each list is empty before the call */
    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);

    /* Save old copies of the whole structures */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* Post‑condition for list ‘a’ (now contains original contents of ‘b’) */
    if (b_was_empty) {
        /* ‘b’ was empty, so ‘a’ must be empty after the swap */
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* ‘b’ was non‑empty, so ‘a’ should now have the same head/tail links as old ‘b’ */
        assert(a.head.next == old_b.head.next);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev == old_b.tail.prev);
        assert(a.tail.prev->next == &a.tail);
    }

    /* Post‑condition for list ‘b’ (now contains original contents of ‘a’) */
    if (a_was_empty) {
        /* ‘a’ was empty, so ‘b’ must be empty after the swap */
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* ‘a’ was non‑empty, so ‘b’ should now have the same head/tail links as old ‘a’ */
        assert(b.head.next == old_a.head.next);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev == old_a.tail.prev);
        assert(b.tail.prev->next == &b.tail);
    }
}
