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

    /* 1. Validity invariants must hold */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));

    /* 2. Empty‑status is swapped */
    assert(aws_linked_list_empty(&a) == aws_linked_list_empty(&old_b));
    assert(aws_linked_list_empty(&b) == aws_linked_list_empty(&old_a));

    /* 3. Sentinel node fields that never change */
    assert(a.head.prev == old_a.head.prev);
    assert(a.tail.next == old_a.tail.next);
    assert(b.head.prev == old_b.head.prev);
    assert(b.tail.next == old_b.tail.next);

    /* 4. Head/ tail linkage after swap */
    struct aws_linked_list_node *expected_a_head_next =
        aws_linked_list_empty(&old_b) ? &a.tail : old_b.head.next;
    struct aws_linked_list_node *expected_a_tail_prev =
        aws_linked_list_empty(&old_b) ? &a.head : old_b.tail.prev;

    struct aws_linked_list_node *expected_b_head_next =
        aws_linked_list_empty(&old_a) ? &b.tail : old_a.head.next;
    struct aws_linked_list_node *expected_b_tail_prev =
        aws_linked_list_empty(&old_a) ? &b.head : old_a.tail.prev;

    assert(a.head.next == expected_a_head_next);
    assert(a.tail.prev == expected_a_tail_prev);
    assert(b.head.next == expected_b_head_next);
    assert(b.tail.prev == expected_b_tail_prev);
}
