#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_swap_contents_harness() {
    /* 1. Declare and allocate two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* 2. Assume both lists are valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* 3. Save old state before the call */
    /* Save key pointers from a and b before swap */
    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);

    struct aws_linked_list_node *a_first = a.head.next;
    struct aws_linked_list_node *a_last  = a.tail.prev;
    struct aws_linked_list_node *b_first = b.head.next;
    struct aws_linked_list_node *b_last  = b.tail.prev;

    /* 4. Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 5. Assert postconditions */

    /* Both lists must remain valid after the swap */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* After swap:
     * - a should contain what b had
     * - b should contain what a had
     */

    /* If b was empty before, a should be empty after */
    if (b_was_empty) {
        assert(aws_linked_list_empty(&a));
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* a should now contain b's old contents */
        assert(!aws_linked_list_empty(&a));
        assert(a.head.next == b_first);
        assert(a.tail.prev == b_last);
        /* Check bidirectional linkage at boundaries */
        assert(b_first->prev == &a.head);
        assert(b_last->next  == &a.tail);
    }

    /* If a was empty before, b should be empty after */
    if (a_was_empty) {
        assert(aws_linked_list_empty(&b));
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* b should now contain a's old contents */
        assert(!aws_linked_list_empty(&b));
        assert(b.head.next == a_first);
        assert(b.tail.prev == a_last);
        /* Check bidirectional linkage at boundaries */
        assert(a_first->prev == &b.head);
        assert(a_last->next  == &b.tail);
    }

    /* Deep validity check */
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));
}
