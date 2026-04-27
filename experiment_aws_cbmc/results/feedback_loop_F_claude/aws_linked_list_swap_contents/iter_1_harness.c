/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_linked_list_swap_contents
 *
 * The function swaps the contents of two linked lists:
 * - dst gets src's nodes, src gets dst's old nodes
 * - Both lists remain valid after the swap
 * - If src was empty, dst becomes empty (and vice versa)
 */
void aws_linked_list_swap_contents_harness() {
    /* Declare two linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;

    /* Allocate and initialize with non-deterministic contents */
    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions: both lists must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Save state before the call */
    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);

    /* Save the first and last nodes of each list before swap */
    struct aws_linked_list_node *a_first = a.head.next;
    struct aws_linked_list_node *a_last = a.tail.prev;
    struct aws_linked_list_node *b_first = b.head.next;
    struct aws_linked_list_node *b_last = b.tail.prev;

    /* Call function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* Assert validity invariants hold after the call */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* Assert postconditions:
     * - a should now contain what b had
     * - b should now contain what a had
     */

    /* If b was empty before, a should be empty now */
    if (b_was_empty) {
        assert(aws_linked_list_empty(&a));
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* a should now start with b's old first node */
        assert(!aws_linked_list_empty(&a));
        assert(a.head.next == b_first);
        assert(b_first->prev == &a.head);
        assert(a.tail.prev == b_last);
        assert(b_last->next == &a.tail);
    }

    /* If a was empty before, b should be empty now */
    if (a_was_empty) {
        assert(aws_linked_list_empty(&b));
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* b should now start with a's old first node */
        assert(!aws_linked_list_empty(&b));
        assert(b.head.next == a_first);
        assert(a_first->prev == &b.head);
        assert(b.tail.prev == a_last);
        assert(a_last->next == &b.tail);
    }

    /* Deep validity check */
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));
}
