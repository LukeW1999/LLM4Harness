#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_swap_contents_harness(void) {
    /* 1. Allocate and bound the two lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* 2. Save old state */
    struct aws_linked_list a_old = a;
    struct aws_linked_list b_old = b;

    struct aws_linked_list_node *a_first_old = a.head.next;
    struct aws_linked_list_node *a_last_old  = a.tail.prev;
    struct aws_linked_list_node *b_first_old = b.head.next;
    struct aws_linked_list_node *b_last_old  = b.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 4. Post‑condition: contents are swapped */

    /* a now contains the old contents of b */
    if (aws_linked_list_empty(&b_old)) {
        /* b was empty → a must be empty */
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        assert(a.head.next == b_old.head.next);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev == b_old.tail.prev);
        assert(a.tail.prev->next == &a.tail);
    }

    /* b now contains the old contents of a */
    if (aws_linked_list_empty(&a_old)) {
        /* a was empty → b must be empty */
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        assert(b.head.next == a_first_old);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev == a_last_old);
        assert(b.tail.prev->next == &b.tail);
    }

    /* 5. Unchanged fields (sentinel links that are not modified) */
    assert(a.head.prev == a_old.head.prev);
    assert(a.tail.next == a_old.tail.next);
    assert(b.head.prev == b_old.head.prev);
    assert(b.tail.next == b_old.tail.next);

    /* 6. Validity invariants must hold after the swap */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));
}
