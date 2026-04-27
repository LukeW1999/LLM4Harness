#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_swap_contents_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list a;
    struct aws_linked_list b;
    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    /* 3. Call function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Success path: both lists are swapped */
    assert(a.head.next == old_b.head.next);
    assert(a.tail.prev == old_b.tail.prev);
    assert(b.head.next == old_a.head.next);
    assert(b.tail.prev == old_a.tail.prev);

    /* If b was empty, a should be initialized */
    if (aws_linked_list_empty(&old_b)) {
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    }

    /* If a was empty, b should be initialized */
    if (old_a.head.next == &old_a.tail) {
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* No other fields are modified in this function */

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
}
