#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>

/**
 * Harness for aws_linked_list_move_all_back:
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
 *   - If src was non-empty, dst is non-empty
 *   - If src was empty, dst is unchanged (same emptiness)
 */
void aws_linked_list_move_all_back_harness(void) {
    /* 1. Declare and initialize two linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* 2. Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    /* dst and src must be different lists */
    __CPROVER_assume(&dst != &src);

    /* 3. Save old state */
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Save the old front/back of dst and src for linkage checks */
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    /* 4. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 5. Assert postconditions */

    /* src must always be empty after the call */
    assert(aws_linked_list_empty(&src));

    /* src head and tail must be properly reset */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* Both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    if (!src_was_empty) {
        /* dst must be non-empty after receiving src's nodes */
        assert(!aws_linked_list_empty(&dst));

        /* The old src_front should now be connected to old_dst_back */
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);

        /* The old src_back should now point to dst's tail */
        assert(dst.tail.prev == old_src_back);
        assert(old_src_back->next == &dst.tail);
    } else {
        /* src was empty: dst should be unchanged */
        assert(dst_was_empty == aws_linked_list_empty(&dst));
        /* dst's back pointer should be unchanged */
        assert(dst.tail.prev == old_dst_back);
    }
}
