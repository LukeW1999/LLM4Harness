Looking at the function `aws_linked_list_move_all_back`:

**Analysis:**
1. **What changes on success (src non-empty):** dst gets src's nodes appended to its back; src becomes empty
2. **What changes when src is empty:** nothing changes (both lists remain as-is)
3. **Unchanged fields:** Both lists remain valid; src becomes empty after the call
4. **Validity invariants:** Both `aws_linked_list_is_valid(dst)` and `aws_linked_list_is_valid(src)` must hold after the call

From the Doxygen: "Remove all nodes from one list, and add them to the back of another. Example: if dst={1,2} and src={3,4}, they become dst={1,2,3,4} and src={}"

Key postconditions:
- `src` is always empty after the call
- `dst` is always valid after the call
- `src` is always valid after the call

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
    /* dst != src (AWS_RESTRICT precondition) */
    __CPROVER_assume(&dst != &src);

    /* Save old state */
    bool src_was_empty = aws_linked_list_empty(&src);
    bool dst_was_empty = aws_linked_list_empty(&dst);

    /* Save pointers for connectivity checks when src is non-empty */
    struct aws_linked_list_node *old_dst_back = dst.tail.prev;
    struct aws_linked_list_node *old_src_front = src.head.next;
    struct aws_linked_list_node *old_src_back = src.tail.prev;

    /* 2. Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 3. Assert postconditions */

    /* Validity invariants always hold */
    assert(aws_linked_list_is_valid(&src));
    assert(aws_linked_list_is_valid(&dst));

    /* src is always empty after the call */
    assert(aws_linked_list_empty(&src));

    /* src's head and tail are properly reset */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    if (!src_was_empty) {
        /* When src was non-empty, its nodes are now in dst */
        /* The old src_front should now be connected after old_dst_back */
        assert(old_dst_back->next == old_src_front);
        assert(old_src_front->prev == old_dst_back);

        /* The old src_back should now point to dst's tail */
        assert(dst.tail.prev == old_src_back);
        assert(old_src_back->next == &dst.tail);

        /* dst is not empty */
        assert(!aws_linked_list_empty(&dst));
    } else {
        /* When src was empty, dst is unchanged */
        if (dst_was_empty) {
            assert(aws_linked_list_empty(&dst));
        } else {
            assert(!aws_linked_list_empty(&dst));
        }
        /* dst's back pointer is unchanged */
        assert(dst.tail.prev == old_dst_back);
    }
}
