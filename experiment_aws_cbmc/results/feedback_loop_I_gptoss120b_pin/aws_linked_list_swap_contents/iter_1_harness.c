#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_swap_contents_harness() {
    /* 1. Allocate and bound the two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* 2. Save old state */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    /* 3. Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 4. Post‑conditions: contents are swapped */

    /* a now contains the old contents of b */
    if (aws_linked_list_empty(&old_b)) {
        /* b was empty → a must be empty */
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        assert(a.head.next == old_b.head.next);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev == old_b.tail.prev);
        assert(a.tail.prev->next == &a.tail);
    }

    /* b now contains the old contents of a */
    if (aws_linked_list_empty(&old_a)) {
        /* a was empty → b must be empty */
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        assert(b.head.next == old_a.head.next);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev == old_a.tail.prev);
        assert(b.tail.prev->next == &b.tail);
    }

    /* 5. Unchanged fields (invariants of the list structure) */
    assert(a.head.prev == &a.head);
    assert(a.tail.next == &a.tail);
    assert(b.head.prev == &b.head);
    assert(b.tail.next == &b.tail);

    /* 6. Validity invariants must hold after the swap */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
}
