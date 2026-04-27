Looking at the function `aws_linked_list_move_all_back`:

**Analysis:**
1. **What changes on success (src non-empty):** dst gets src's nodes appended to back; src becomes empty. Specifically: dst's last node connects to src's first node, src's last node connects to dst's tail, src is reset to empty.
2. **What changes when src is empty:** Nothing changes (early return).
3. **Unchanged fields:** Both lists remain valid linked lists.
4. **Validity invariants:** Both `src` and `dst` must be valid after the call.

From the Doxygen: "Remove all nodes from one list, and add them to the back of another. Example: if dst={1,2} and src={3,4}, they become dst={1,2,3,4} and src={}"

Key postconditions:
- `src` is always empty after the call
- `dst` is valid after the call
- If src was empty, dst is unchanged
- If src was non-empty, dst's tail.prev is the old src_back, and src is empty

```c
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Declare and initialize two distinct linked lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* 2. Save old state before calling */
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Save pointers for postcondition checks */
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;
    struct aws_linked_list_node *old_dst_front = dst.head.next;

    /* 3. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Assert postconditions */

    /* src must always be empty after the call */
    assert(aws_linked_list_empty(&src));

    /* src must be properly reset: head.next == &src.tail and tail.prev == &src.head */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* Both lists must be valid */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    if (!src_was_empty) {
        /* dst's tail.prev should now be old src_back */
        assert(dst.tail.prev == old_src_back);
        /* old src_back's next should point to dst's tail */
        assert(old_src_back->next == &dst.tail);

        /* old dst_back should connect to old src_front */
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);

        /* dst is not empty */
        assert(!aws_linked_list_empty(&dst));

        /* dst's head.next should be unchanged (old dst front or old src front if dst was empty) */
        if (dst_was_empty) {
            /* dst was empty, so dst.head.next was &dst.tail (old_dst_back == &dst.head) */
            /* After move, dst.head.next should be old_src_front */
            assert(dst.head.next == old_src_front);
            assert(old_src_front->prev == &dst.head);
        } else {
            /* dst was non-empty, head.next unchanged */
            assert(dst.head.next == old_dst_front);
        }
    } else {
        /* src was empty: dst should be unchanged */
        assert(dst.head.next == old_dst_front);
        assert(dst.tail.prev == old_dst_back);
    }
}
