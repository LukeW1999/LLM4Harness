#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

/* Harness for aws_linked_list_move_all_back */
void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the destination and source linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_last = dst.tail.prev;   /* may be &dst.head if dst empty */
    struct aws_linked_list_node *old_src_front = src.head.next; /* may be &src.tail if src empty */
    struct aws_linked_list_node *old_src_back  = src.tail.prev; /* may be &src.head if src empty */

    bool src_was_empty = (src.head.next == &src.tail);

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition checks */

    /* validity invariants must hold for both lists */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* head.prev and tail.next are immutable for a well‑formed list */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (src_was_empty) {
        /* when src is empty, dst must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
        /* src must remain empty */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);
    } else {
        /* src must be empty after the move */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        /* dst must have src's nodes appended */
        assert(dst.tail.prev == old_src_back);
        assert(old_src_back->next == &dst.tail);
        assert(old_src_front->prev == old_dst_last);
        assert(old_dst_last->next == old_src_front);
    }

    /* unchanged fields for dst that are not affected by the operation */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    /* unchanged fields for src that are not affected by the operation */
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
