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

    /* 2. Save old copies for post‑condition checks */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    /* 3. Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 4. Post‑conditions: validity must hold for both lists */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));

    /* 5. Post‑conditions: contents are swapped */
    if (aws_linked_list_empty(&old_b)) {
        /* b was empty, a must become empty */
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* a now contains the former contents of b */
        assert(a.head.next == old_b.head.next);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev == old_b.tail.prev);
        assert(a.tail.prev->next == &a.tail);
    }

    if (aws_linked_list_empty(&old_a)) {
        /* a was empty, b must become empty */
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* b now contains the former contents of a */
        assert(b.head.next == old_a.head.next);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev == old_a.tail.prev);
        assert(b.tail.prev->next == &b.tail);
    }

    /* 6. Unchanged structural fields (the list objects themselves) */
    assert(&a.head == &old_a.head);
    assert(&a.tail == &old_a.tail);
    assert(&b.head == &old_b.head);
    assert(&b.tail == &old_b.tail);
}
