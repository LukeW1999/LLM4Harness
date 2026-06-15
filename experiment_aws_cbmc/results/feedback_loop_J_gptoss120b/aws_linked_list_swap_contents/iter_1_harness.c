#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_swap_contents_harness() {
    /* 1. Allocate and assume validity of the two lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* 2. Save old state */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    struct aws_linked_list_node *old_a_head_prev = a.head.prev;
    struct aws_linked_list_node *old_a_tail_next = a.tail.next;
    struct aws_linked_list_node *old_b_head_prev = b.head.prev;
    struct aws_linked_list_node *old_b_tail_next = b.tail.next;

    /* 3. Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 4. Unchanged fields (head.prev and tail.next are never modified) */
    assert(a.head.prev == old_a_head_prev);
    assert(a.tail.next == old_a_tail_next);
    assert(b.head.prev == old_b_head_prev);
    assert(b.tail.next == old_b_tail_next);

    /* 5. Post‑conditions depending on the emptiness of the original lists */
    if (aws_linked_list_empty(&old_b)) {
        /* b was empty → a becomes empty */
        assert(aws_linked_list_empty(&a));

        /* b now contains the former contents of a (if any) */
        if (!aws_linked_list_empty(&old_a)) {
            assert(b.head.next == old_a.head.next);
            assert(b.tail.prev == old_a.tail.prev);
            assert(b.head.next->prev == &b.head);
            assert(b.tail.prev->next == &b.tail);
        } else {
            /* both were empty */
            assert(aws_linked_list_empty(&b));
        }
    } else {
        /* b was non‑empty → a now contains b's former contents */
        assert(a.head.next == old_b.head.next);
        assert(a.tail.prev == old_b.tail.prev);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev->next == &a.tail);

        if (aws_linked_list_empty(&old_a)) {
            /* a was empty → b becomes empty */
            assert(aws_linked_list_empty(&b));
        } else {
            /* both were non‑empty → b now contains a's former contents */
            assert(b.head.next == old_a.head.next);
            assert(b.tail.prev == old_a.tail.prev);
            assert(b.head.next->prev == &b.head);
            assert(b.tail.prev->next == &b.tail);
        }
    }

    /* 6. Validity invariants must hold for both lists */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
}
