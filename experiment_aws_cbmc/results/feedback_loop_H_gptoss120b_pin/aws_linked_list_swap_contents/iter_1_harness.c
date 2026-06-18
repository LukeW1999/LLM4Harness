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

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* Validity invariants must hold after the call */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* Unchanged fields (head.prev and tail.next are never modified) */
    assert(a.head.prev == old_a.head.prev);
    assert(a.tail.next == old_a.tail.next);
    assert(b.head.prev == old_b.head.prev);
    assert(b.tail.next == old_b.tail.next);

    /* Post‑conditions for list a based on whether the original b was empty */
    if (old_b.head.next == &old_b.tail) {
        /* b was empty → a becomes empty */
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* a receives b's former contents */
        assert(a.head.next == old_b.head.next);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev == old_b.tail.prev);
        assert(a.tail.prev->next == &a.tail);
    }

    /* Post‑conditions for list b based on whether the original a was empty */
    if (old_a.head.next == &old_a.tail) {
        /* a was empty → b becomes empty */
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* b receives a's former contents */
        assert(b.head.next == old_a.head.next);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev == old_a.tail.prev);
        assert(b.tail.prev->next == &b.tail);
    }
}
