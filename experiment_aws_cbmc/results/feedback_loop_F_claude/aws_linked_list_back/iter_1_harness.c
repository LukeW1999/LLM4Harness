/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_linked_list_back
 *
 * aws_linked_list_back returns the element at the back of the list,
 * which is list->tail.prev.
 *
 * Preconditions:
 *   - list is valid
 *   - list is non-empty (back requires at least one element)
 *
 * Postconditions:
 *   - returns list->tail.prev
 *   - list is unchanged
 *   - list remains valid
 */
void aws_linked_list_back_harness() {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Precondition: list must be non-empty for back() to be valid */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 3. Save old state before calling */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* 5. Assert postconditions */

    /* The return value must be list->tail.prev */
    assert(result == old_tail_prev);
    assert(result == list.tail.prev);

    /* The list structure must be unchanged */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    /* The list must still be non-empty */
    assert(!aws_linked_list_empty(&list));

    /* The result must not be NULL (since list is non-empty, tail.prev != &list.head... 
       actually tail.prev points to the last real node, not head, since list is non-empty) */
    assert(result != NULL);

    /* 6. Assert validity invariant still holds */
    assert(aws_linked_list_is_valid(&list));
}
