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

    /* Assume both lists are valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* 2. Save old state before calling */
    /* Save key pointers from a and b before swap */
    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);

    struct aws_linked_list_node *a_old_first = a.head.next;
    struct aws_linked_list_node *a_old_last  = a.tail.prev;
    struct aws_linked_list_node *b_old_first = b.head.next;
    struct aws_linked_list_node *b_old_last  = b.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 4. Assert postconditions */

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
        /* a should now contain b's old contents */
        assert(!aws_linked_list_empty(&a));
        assert(a.head.next == b_old_first);
        assert(a.tail.prev == b_old_last);
        /* Check bidirectional linkage */
        assert(a.head.next->prev == &a.head);
        assert(a.tail.prev->next == &a.tail);
    }

    /* If a was empty, b should now be empty */
    if (a_was_empty) {
        assert(aws_linked_list_empty(&b));
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* b should now contain a's old contents */
        assert(!aws_linked_list_empty(&b));
        assert(b.head.next == a_old_first);
        assert(b.tail.prev == a_old_last);
        /* Check bidirectional linkage */
        assert(b.head.next->prev == &b.head);
        assert(b.tail.prev->next == &b.tail);
    }

    /* 5. Assert validity invariants hold after the call */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
}
