#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

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
    /* Success path: both lists are swapped correctly */
    assert(a.head.next == old_b.head.next);
    assert(a.tail.prev == old_b.tail.prev);
    assert(b.head.next == old_a.head.next);
    assert(b.tail.prev == old_a.tail.prev);

    /* 5. Assert fields that must NOT change regardless of result */
    /* No additional fields to assert as all fields are involved in the swap */

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
}
