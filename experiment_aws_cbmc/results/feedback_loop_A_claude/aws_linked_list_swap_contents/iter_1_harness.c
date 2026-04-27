Looking at the function `aws_linked_list_swap_contents`, I need to analyze:

1. **What changes**: The contents of lists `a` and `b` are swapped. After the call, `a` contains what `b` had, and `b` contains what `a` had.
2. **What's unchanged**: The list structures themselves (head/tail nodes are embedded, not pointers), but their `next`/`prev` pointers change to reflect swapped contents.
3. **Failure**: This function has no failure path (void return).
4. **Validity invariants**: Both `a` and `b` must be valid linked lists after the call.

Key postconditions:
- If `b` was empty before, `a` is empty after; otherwise `a`'s head.next points to what was `b`'s first node, and `a`'s tail.prev points to what was `b`'s last node.
- If `a` was empty before, `b` is empty after; otherwise `b`'s head.next points to what was `a`'s first node, and `b`'s tail.prev points to what was `a`'s last node.
- Both lists remain valid after the swap.

```c
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_swap_contents_harness(void) {
    /* 1. Declare and initialize two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* 2. Assume both lists are valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* 3. Save state before the call */
    /* Save whether each list was empty */
    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);

    /* Save the first and last nodes of each list */
    struct aws_linked_list_node *a_first = a.head.next;
    struct aws_linked_list_node *a_last  = a.tail.prev;
    struct aws_linked_list_node *b_first = b.head.next;
    struct aws_linked_list_node *b_last  = b.tail.prev;

    /* 4. Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 5. Assert postconditions */

    /* Both lists must be valid after the swap */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* Check what A contains after swap (should be B's old contents) */
    if (b_was_empty) {
        /* A should now be empty */
        assert(aws_linked_list_empty(&a));
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* A should contain B's old contents */
        assert(!aws_linked_list_empty(&a));
        assert(a.head.next == b_first);
        assert(a.tail.prev == b_last);
        /* Check bidirectional linkage at boundaries */
        assert(b_first->prev == &a.head);
        assert(b_last->next == &a.tail);
    }

    /* Check what B contains after swap (should be A's old contents) */
    if (a_was_empty) {
        /* B should now be empty */
        assert(aws_linked_list_empty(&b));
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* B should contain A's old contents */
        assert(!aws_linked_list_empty(&b));
        assert(b.head.next == a_first);
        assert(b.tail.prev == a_last);
        /* Check bidirectional linkage at boundaries */
        assert(a_first->prev == &b.head);
        assert(a_last->next == &b.tail);
    }

    /* Deep validity check */
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));
}
