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
    /* 1. Set up dst */
    struct aws_linked_list dst;
    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    /* 2. Set up src */
    struct aws_linked_list src;
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* 3. dst != src (restrict aliasing) */
    __CPROVER_assume(&dst != &src);

    /* 4. Snapshot state before the call */
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Save key pointers before the call */
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_dst_front = dst.head.next;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    /* 5. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 6. Postcondition: both lists are valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 7. Postcondition: src is always empty after the call */
    assert(aws_linked_list_empty(&src));

    /* 8. Postcondition: src's head and tail are properly reset */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (!src_was_empty) {
        /* 9. When src was non-empty:
         *    - old_src_front is now linked after old_dst_back
         *    - old_src_back is now dst's tail.prev
         *    - old_dst_back->next == old_src_front
         *    - old_src_front->prev == old_dst_back
         *    - dst->tail.prev == old_src_back
         *    - old_src_back->next == &dst.tail
         */
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);
        assert(dst.tail.prev == old_src_back);
        assert(old_src_back->next == &dst.tail);

        /* dst's head.next is unchanged (we only appended to the back) */
        assert(dst.head.next == old_dst_front);
    } else {
        /* 10. When src was empty: dst is completely unchanged */
        assert(dst.head.next == old_dst_front);
        assert(dst.tail.prev == old_dst_back);
    }

    /* 11. dst head/tail structural invariants */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
}
