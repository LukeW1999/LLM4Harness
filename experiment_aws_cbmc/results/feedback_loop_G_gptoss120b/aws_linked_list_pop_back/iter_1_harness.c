#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the source and destination lists */
    struct aws_linked_list src;
    struct aws_linked_list dst;

    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(&src != &dst);

    /* 2. Save old state */
    struct aws_linked_list old_src = src;
    struct aws_linked_list old_dst = dst;

    struct aws_linked_list_node *old_src_first = src.head.next;
    struct aws_linked_list_node *old_src_last  = src.tail.prev;
    struct aws_linked_list_node *old_dst_first = dst.head.next;
    struct aws_linked_list_node *old_dst_last  = dst.tail.prev;

    bool src_was_empty = (src.head.next == &src.tail);

    /* 3. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Post‑condition assertions */

    /* Validity invariants must hold for both lists */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    if (!src_was_empty) {
        /* src must be empty after the move */
        assert(src.head.next == &src.tail);
        assert(src.tail.prev == &src.head);

        /* dst must contain the original dst elements followed by the original src elements */
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
        assert(dst.tail.prev == old_src_last);
    } else {
        /* src was already empty, dst must be unchanged */
        assert(dst.head.next == old_dst_first);
        assert(dst.tail.prev == old_dst_last);
    }

    /* Unchanged fields for src when it was empty */
    if (src_was_empty) {
        assert(src.head.prev == old_src.head.prev);
        assert(src.tail.next == old_src.tail.next);
    }

    /* Unchanged fields for dst when src was empty */
    if (src_was_empty) {
        assert(dst.head.prev == old_dst.head.prev);
        assert(dst.tail.next == old_dst.tail.next);
    }

    /* The head and tail node structures themselves should remain zeroed except for their linkage fields */
    /* (No additional fields exist in aws_linked_list_node, so no further unchanged assertions are needed) */
}
