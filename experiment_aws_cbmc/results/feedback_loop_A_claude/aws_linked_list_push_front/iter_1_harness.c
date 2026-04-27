Looking at the function `aws_linked_list_move_all_back`:

**Analysis:**
1. **What changes on success:** If src is non-empty, src nodes are moved to the back of dst. dst's tail.prev becomes src's old back node, src becomes empty (head.next == &src->tail, tail.prev == &src->head).
2. **What's unchanged:** The nodes themselves (their data), just their linkage changes. Both lists remain valid.
3. **Failure:** No failure path - void function.
4. **Validity invariants:** Both src and dst must be valid after the call. src must be empty after the call.

The Doxygen says: "Remove all nodes from one list, and add them to the back of another. Example: if dst={1,2} and src={3,4}, they become dst={1,2,3,4} and src={}"

So postconditions:
- src is always empty after the call
- dst and src are both valid
- dst != src (precondition)

```c
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_move_all_back_harness(void) {
    /* Declare and initialize two linked lists */
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
    struct aws_linked_list_node *src_old_front = src.head.next;
    struct aws_linked_list_node *src_old_back = src.tail.prev;
    struct aws_linked_list_node *dst_old_back = dst.tail.prev;

    /* Call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* Postcondition 1: Both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Postcondition 2: src is always empty after the call */
    assert(aws_linked_list_empty(&src));
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* Postcondition 3: If src was non-empty, dst's back is src's old back node */
    if (!src_was_empty) {
        assert(dst.tail.prev == src_old_back);
        assert(src_old_back->next == &dst.tail);
        assert(dst_old_back->next == src_old_front);
        assert(src_old_front->prev == dst_old_back);
    } else {
        /* If src was empty, dst is unchanged */
        assert(dst.tail.prev == dst_old_back);
    }

    /* Postcondition 4: src head and tail internal pointers are valid */
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* Postcondition 5: dst head and tail internal pointers are valid */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
}
