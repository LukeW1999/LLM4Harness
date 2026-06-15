#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_head_next = dst.head.next;
    struct aws_linked_list_node *old_dst_tail_prev = dst.tail.prev;
    struct aws_linked_list_node *old_src_head_next = src.head.next;
    struct aws_linked_list_node *old_src_tail_prev = src.tail.prev;

    bool src_was_empty = (src.head.next == &src.tail);
    bool dst_was_empty = (dst.head.next == &dst.tail);

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Postconditions */

    /* Validity invariants must hold for both lists */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Source list must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    if (src_was_empty) {
        /* No change to destination when source was empty */
        assert(dst.head.next == old_dst_head_next);
        assert(dst.tail.prev == old_dst_tail_prev);
    } else {
        /* Destination tail now points to the former source tail */
        assert(dst.tail.prev == old_src_tail_prev);
        assert(old_src_tail_prev->next == &dst.tail);

        if (dst_was_empty) {
            /* Destination head now points to the former source head */
            assert(dst.head.next == old_src_head_next);
            assert(old_src_head_next->prev == &dst.head);
        } else {
            /* Destination head unchanged */
            assert(dst.head.next == old_dst_head_next);
            /* The node that was previously the last in dst now links to the former source head */
            assert(old_dst_tail_prev->next == old_src_head_next);
            assert(old_src_head_next->prev == old_dst_tail_prev);
        }
    }

    /* Unchanged fields of the list structures (except head/tail links) */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
