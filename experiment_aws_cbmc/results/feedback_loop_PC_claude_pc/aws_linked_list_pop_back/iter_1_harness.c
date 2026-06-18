#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Harness for aws_linked_list_move_all_back:
 *
 * Remove all nodes from src, and add them to the back of dst.
 * Example: if dst={1,2} and src={3,4}, they become dst={1,2,3,4} and src={}
 *
 * Preconditions:
 *   - dst is a valid linked list
 *   - src is a valid linked list
 *   - dst != src
 *
 * Postconditions:
 *   - src is empty after the call
 *   - dst is valid after the call
 *   - src is valid after the call
 *   - If src was non-empty before the call:
 *       - The old src_front is now the first node after dst's old back
 *       - The old src_back is now dst's tail.prev
 *   - If src was empty before the call:
 *       - dst is unchanged (same head.next and tail.prev)
 */
void aws_linked_list_move_all_back_harness(void) {
    /* 1. Declare and initialize dst and src */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    /* dst != src (restrict aliasing) */
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state before the call */
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Save pointers to src's front and back nodes (before the call) */
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back  = src.tail.prev;

    /* Save dst's old back node */
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;

    /* Save dst's old front node */
    struct aws_linked_list_node *old_dst_front = dst.head.next;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Assert postconditions */

    /* INVARIANTS: both lists must be valid after the call */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src must be empty after the call */
    assert(aws_linked_list_empty(&src));

    /* src's head.next must point to src's tail (empty list) */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    if (!src_was_empty) {
        /* If src was non-empty:
         * - old_dst_back->next should now be old_src_front
         * - old_src_front->prev should now be old_dst_back
         * - dst->tail.prev should now be old_src_back
         * - old_src_back->next should now be &dst.tail
         */
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);
        assert(dst.tail.prev == old_src_back);
        assert(old_src_back->next == &dst.tail);

        /* dst's head.next should be unchanged (we only appended to back) */
        assert(dst.head.next == old_dst_front);
    } else {
        /* If src was empty, dst should be completely unchanged */
        assert(dst.head.next == old_dst_front);
        assert(dst.tail.prev == old_dst_back);

        /* If dst was also empty, it should still be empty */
        if (dst_was_empty) {
            assert(aws_linked_list_empty(&dst));
        }
    }
}
