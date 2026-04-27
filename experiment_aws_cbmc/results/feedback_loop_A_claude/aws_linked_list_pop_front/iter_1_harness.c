Looking at the function `aws_linked_list_move_all_back`:

**Analysis:**
1. **What changes on success (src non-empty):** dst gets src's nodes appended to back; src becomes empty
2. **What's unchanged:** The nodes themselves aren't freed; dst and src remain valid lists
3. **What happens when src is empty:** Nothing changes, both lists remain as-is
4. **Validity invariants:** Both `dst` and `src` must be valid after the call

From the Doxygen: "Remove all nodes from one list, and add them to the back of another. Example: if dst={1,2} and src={3,4}, they become dst={1,2,3,4} and src={}"

Key postconditions:
- `src` is always empty after the call
- `dst` is always valid after the call
- `src` is always valid after the call
- If src was non-empty, dst is non-empty after

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

    /* Save old state */
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Save pointers to src's front and back nodes (if non-empty) */
    struct aws_linked_list_node *src_front = src.head.next;
    struct aws_linked_list_node *src_back = src.tail.prev;
    struct aws_linked_list_node *dst_back = dst.tail.prev;

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 4. Validity invariants */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 1. Changed fields: src is always empty after the call */
    assert(aws_linked_list_empty(&src));

    /* src's head and tail are properly reset */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    if (!src_was_empty) {
        /* dst should not be empty since we moved nodes into it */
        assert(!aws_linked_list_empty(&dst));

        /* The src_front node is now the first node after dst's old back */
        assert(dst_back->next == src_front);
        assert(src_front->prev == dst_back);

        /* The src_back node is now dst's last node before tail */
        assert(dst.tail.prev == src_back);
        assert(src_back->next == &dst.tail);
    } else {
        /* src was empty: dst should be unchanged */
        if (dst_was_empty) {
            assert(aws_linked_list_empty(&dst));
        }
        /* dst's tail.prev should still be dst_back */
        assert(dst.tail.prev == dst_back);
    }

    /* 2. Unchanged fields: head.prev and tail.next are always NULL in a valid list */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
