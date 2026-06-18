#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_swap_contents_harness() {
    /* 1. Declare and initialize two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* 2. Assume both lists are valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* 3. Save old state before the call */
    /* Save key pointers from a and b before swap */
    struct aws_linked_list_node *a_first = a.head.next;
    struct aws_linked_list_node *a_last  = a.tail.prev;
    struct aws_linked_list_node *b_first = b.head.next;
    struct aws_linked_list_node *b_last  = b.tail.prev;

    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);

    /* 4. Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 5. Assert postconditions */

    /* After swap:
     * - a should contain what b had
     * - b should contain what a had
     */

    /* If b was empty, a should now be empty */
    if (b_was_empty) {
        assert(aws_linked_list_empty(&a));
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* a should now start with b's first node */
        assert(a.head.next == b_first);
        assert(b_first->prev == &a.head);
        /* a should now end with b's last node */
        assert(a.tail.prev == b_last);
        assert(b_last->next == &a.tail);
        /* a is not empty */
        assert(!aws_linked_list_empty(&a));
    }

    /* If a was empty, b should now be empty */
    if (a_was_empty) {
        assert(aws_linked_list_empty(&b));
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* b should now start with a's old first node */
        assert(b.head.next == a_first);
        assert(a_first->prev == &b.head);
        /* b should now end with a's old last node */
        assert(b.tail.prev == a_last);
        assert(a_last->next == &b.tail);
        /* b is not empty */
        assert(!aws_linked_list_empty(&b));
    }

    /* 6. Assert validity invariants hold after the call */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* 7. Assert head/tail sentinel linkage invariants */
    /* a's head and tail must point to each other or through valid nodes */
    assert(a.head.next != NULL);
    assert(a.tail.prev != NULL);
    assert(b.head.next != NULL);
    assert(b.tail.prev != NULL);

    /* head.prev and tail.next are not used by the list but should remain consistent */
    /* The head sentinel's prev and tail sentinel's next are not modified by swap */
    /* (they are set by aws_linked_list_init if empty, otherwise not touched) */

    /* Verify the bidirectional linkage at boundaries */
    assert(a.head.next->prev == &a.head);
    assert(a.tail.prev->next == &a.tail);
    assert(b.head.next->prev == &b.head);
    assert(b.tail.prev->next == &b.tail);
}
