/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_swap_contents_harness() {
    /* data structures */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume both lists are valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Save old state */
    bool a_was_empty = aws_linked_list_empty(&a);
    bool b_was_empty = aws_linked_list_empty(&b);

    /* Save old first/last nodes of each list */
    struct aws_linked_list_node *a_old_first = a.head.next;
    struct aws_linked_list_node *a_old_last = a.tail.prev;
    struct aws_linked_list_node *b_old_first = b.head.next;
    struct aws_linked_list_node *b_old_last = b.tail.prev;

    /* perform operation under verification */
    aws_linked_list_swap_contents(&a, &b);

    /* Both lists must be valid after the swap */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* After swap:
     * - a should contain what b had
     * - b should contain what a had
     */

    /* Check a's new contents (what b had) */
    if (b_was_empty) {
        /* a should now be empty */
        assert(aws_linked_list_empty(&a));
        assert(a.head.next == &a.tail);
        assert(a.tail.prev == &a.head);
    } else {
        /* a should now contain b's old contents */
        assert(!aws_linked_list_empty(&a));
        assert(a.head.next == b_old_first);
        assert(a.tail.prev == b_old_last);
        assert(b_old_first->prev == &a.head);
        assert(b_old_last->next == &a.tail);
    }

    /* Check b's new contents (what a had) */
    if (a_was_empty) {
        /* b should now be empty */
        assert(aws_linked_list_empty(&b));
        assert(b.head.next == &b.tail);
        assert(b.tail.prev == &b.head);
    } else {
        /* b should now contain a's old contents */
        assert(!aws_linked_list_empty(&b));
        assert(b.head.next == a_old_first);
        assert(b.tail.prev == a_old_last);
        assert(a_old_first->prev == &b.head);
        assert(a_old_last->next == &b.tail);
    }

    /* head.prev and tail.next are not used by the list but should remain consistent */
    /* The sentinel nodes' internal linkage should be valid */
    assert(aws_linked_list_node_next_is_valid(&a.head));
    assert(aws_linked_list_node_prev_is_valid(&a.tail));
    assert(aws_linked_list_node_next_is_valid(&b.head));
    assert(aws_linked_list_node_prev_is_valid(&b.tail));
}
