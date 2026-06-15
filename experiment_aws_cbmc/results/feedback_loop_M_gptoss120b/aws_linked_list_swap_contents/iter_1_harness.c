#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_swap_contents_harness(void) {
    /* 1. Allocate and bound the two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* 2. Save old state for later comparison */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);

    /* 3. Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 4. Post‑conditions for list *a* (receives B's original contents) */
    if (b_was_empty) {
        /* B was empty → A must become empty */
        assert(aws_linked_list_empty(&a));
    } else {
        /* A now contains the nodes that were in B */
        assert(a.head.next == old_b.head.next);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev == old_b.tail.prev);
        assert(a.tail.prev->next == &a.tail);
    }

    /* 5. Post‑conditions for list *b* (receives A's original contents) */
    if (a_was_empty) {
        /* A was empty → B must become empty */
        assert(aws_linked_list_empty(&b));
    } else {
        /* B now contains the nodes that were in A */
        assert(b.head.next == old_a.head.next);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev == old_a.tail.prev);
        assert(b.tail.prev->next == &b.tail);
    }

    /* 6. Unchanged sentinel fields */
    assert(a.head.prev == &a.head);
    assert(a.tail.next == &a.tail);
    assert(b.head.prev == &b.head);
    assert(b.tail.next == &b.tail);

    /* 7. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));
}
