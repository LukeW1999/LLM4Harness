#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *old_dst_first = dst.head.next;
    struct aws_linked_list_node *old_dst_last  = dst.tail.prev;
    struct aws_linked_list_node *old_src_first = src.head.next;
    struct aws_linked_list_node *old_src_last  = src.tail.prev;

    bool old_dst_empty = (old_dst.head.next == &old_dst.tail);
    bool old_src_empty = (old_src.head.next == &old_src.tail);

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Validity invariants must hold */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* head.prev and tail.next are immutable for both lists */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (old_src_empty) {
        /* No change to dst when src was empty */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* dst now ends with the former src last node */
        assert(dst.tail.prev == old_src_last);

        if (old_dst_empty) {
            /* dst was empty, its first node is now src's first node */
            assert(dst.head.next == old_src_first);
        } else {
            /* The previous last node of dst should now point to src's first node */
            assert(old_dst_last->next == old_src_first);
        }

        /* src's former first node should now link back to the node that preceded it in dst */
        if (old_dst_empty) {
            assert(old_src_first->prev == &dst.head);
        } else {
            assert(old_src_first->prev == old_dst_last);
        }
    }
}
