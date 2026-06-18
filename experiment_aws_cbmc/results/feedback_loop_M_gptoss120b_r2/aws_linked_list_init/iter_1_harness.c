#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_move_all_back_harness(void) {
    /* Allocate and bound the destination list */
    struct aws_linked_list dst;
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    /* Allocate and bound the source list */
    struct aws_linked_list src;
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* Save old copies of the lists */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* Save pointers to key nodes in the old state */
    struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;   /* may be &old_dst.tail if empty */
    struct aws_linked_list_node *old_src_first = old_src.head.next; /* may be &old_src.tail if empty */
    struct aws_linked_list_node *old_src_last  = old_src.tail.prev;  /* may be &old_src.head if empty */

    /* Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Post‑condition: source list must be empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* Determine whether source was empty before the call */
    if (old_src.head.next == &old_src.tail) {
        /* Source was empty: destination must be unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* Source was non‑empty: destination must contain original dst elements
         * followed by original src elements. */
        /* The first element of dst is unchanged */
        assert(dst.head.next == old_dst.head.next);

        /* The last element of dst is now the last element of the original src */
        assert(dst.tail.prev == old_src_last);

        /* The node that was previously the last element of dst now points to the
         * first element of the original src */
        assert(old_dst_last->next == old_src_first);
        assert(old_src_first->prev == old_dst_last);
    }
}
