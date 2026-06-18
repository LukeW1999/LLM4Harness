#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_swap_contents_harness(void) {
    /* 1. Allocate and bound the two lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* The two lists must be distinct (restrict qualifier) */
    __CPROVER_assume(&a != &b);

    /* 2. Save old state */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    /* 3. Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 4. Postconditions */

    /* 4.1. Invariants: both lists must remain valid */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));

    /* 4.2. Unchanged fields (head.prev and tail.next are never modified) */
    assert(a.head.prev == old_a.head.prev);
    assert(a.tail.next == old_a.tail.next);
    assert(b.head.prev == old_b.head.prev);
    assert(b.tail.next == old_b.tail.next);

    /* 4.3. Emptiness is swapped */
    assert(aws_linked_list_empty(&a) == aws_linked_list_empty(&old_b));
    assert(aws_linked_list_empty(&b) == aws_linked_list_empty(&old_a));

    /* 4.4. When a is not empty after the swap, its head/tail links must match old_b */
    if (!aws_linked_list_empty(&a)) {
        assert(a.head.next == old_b.head.next);
        assert(a.tail.prev == old_b.tail.prev);
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev->next == &a.tail);
    } else {
        /* Empty list representation */
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    }

    /* 4.5. When b is not empty after the swap, its head/tail links must match old_a */
    if (!aws_linked_list_empty(&b)) {
        assert(b.head.next == old_a.head.next);
        assert(b.tail.prev == old_a.tail.prev);
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev->next == &b.tail);
    } else {
        /* Empty list representation */
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    }
}
