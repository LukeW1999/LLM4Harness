#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_swap_contents_harness() {
    /* Allocate and bound the two lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Pre‑conditions: both lists must be valid and distinct */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));
    __CPROVER_assume(&a != &b);

    /* Save old state for post‑condition checks */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    bool a_was_empty = aws_linked_list_empty(&old_a);
    bool b_was_empty = aws_linked_list_empty(&old_b);

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 1. Validity invariants must hold after the call */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* 2. Post‑conditions for list *a* (receives B's former contents) */
    if (b_was_empty) {
        /* B was empty → A becomes empty */
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* A now contains the nodes that were in B */
        assert(a.head.next == old_b.head.next);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev == old_b.tail.prev);
        assert(a.tail.prev->next == &a.tail);
    }

    /* 3. Post‑conditions for list *b* (receives A's former contents) */
    if (a_was_empty) {
        /* A was empty → B becomes empty */
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* B now contains the nodes that were in A */
        assert(b.head.next == old_a.head.next);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev == old_a.tail.prev);
        assert(b.tail.prev->next == &b.tail);
    }

    /* 4. Unchanged fields (the list structures themselves have no other fields) */
    /* No additional fields to assert unchanged. */
}
