#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_move_all_back_harness() {
    /* Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume the lists are initially valid and distinct */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* Save old state for comparison */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_head_next = dst.head.next;
    struct aws_linked_list_node *old_dst_tail_prev = dst.tail.prev;
    struct aws_linked_list_node *old_src_head_next = src.head.next;
    struct aws_linked_list_node *old_src_tail_prev = src.tail.prev;

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑condition: source list must be empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(aws_linked_list_empty(&src));

    /* Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* If the source list was empty, destination list must be unchanged */
    if (old_src_head_next == &old_src.tail) {
        assert(dst.head.next == old_dst_head_next);
        assert(dst.tail.prev == old_dst_tail_prev);
    } else {
        /* Source was non‑empty: destination tail should now be the former source tail */
        assert(dst.tail.prev == old_src_tail_prev);
        /* Destination head should remain the same */
        assert(dst.head.next == old_dst_head_next);
        /* The splice must have linked the old destination tail to the old source head */
        assert(old_dst_tail_prev->next == old_src_head_next);
        assert(old_src_head_next->prev == old_dst_tail_prev);
    }
}
