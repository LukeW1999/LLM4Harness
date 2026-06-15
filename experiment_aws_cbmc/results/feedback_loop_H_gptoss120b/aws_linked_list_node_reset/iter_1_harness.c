#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the two lists */
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

    struct aws_linked_list_node *old_dst_tail = old_dst.tail.prev;   /* may be &old_dst.tail if dst empty */
    struct aws_linked_list_node *old_src_front = old_src.head.next; /* may be &old_src.tail if src empty */
    struct aws_linked_list_node *old_src_back  = old_src.tail.prev; /* may be &old_src.head if src empty */

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑conditions */

    /* validity invariants */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    /* head.prev and tail.next of src remain unchanged (NULL) */
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* dst invariants that never change */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);

    /* If src was empty, dst must be unchanged */
    if (old_src_front == &old_src.tail) {
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* src non‑empty: dst now ends with the former src nodes */
        /* the first node of src is linked after the old last node of dst */
        assert(old_dst_tail->next == old_src_front);
        assert(old_src_front->prev == old_dst_tail);

        /* the former last node of src is now the node before dst.tail */
        assert(dst.tail.prev == old_src_back);
        assert(old_src_back->next == &dst.tail);

        /* the beginning of dst is unchanged */
        assert(dst.head.next == old_dst.head.next);
    }
}
