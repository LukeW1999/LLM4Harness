#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Harness for aws_linked_list_move_all_back */
void aws_linked_list_move_all_back_harness() {
    /* Allocate and bound source and destination linked lists */
    struct aws_linked_list src;
    struct aws_linked_list dst;

    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(&src != &dst);

    /* Save old copies for immutability checks */
    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Postconditions */

    /* Validity invariants must hold for both lists */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* Source list must be empty after the move */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* Destination list changes depend on whether source was originally empty */
    if (aws_linked_list_empty(&old_src)) {
        /* If source was empty, destination should be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* If source was non‑empty, the last node of the original source becomes the new tail predecessor */
        assert(dst.tail.prev == old_src.tail.prev);
        /* The new last node must point to the list tail */
        assert(old_src.tail.prev->next == &dst.tail);
        /* The node that was previously the last of the original destination now points forward to the first node of the original source */
        assert(old_dst.tail.prev->next == old_src.head.next);
        /* The first node of the original source must have its prev pointer updated to the previous last node of the original destination */
        assert(old_src.head.next->prev == old_dst.tail.prev);
    }

    /* Unchanged fields for both lists (head and tail node pointers themselves) */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
