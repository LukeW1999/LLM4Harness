/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_linked_list_front
 *
 * aws_linked_list_front returns the element in the front of the list,
 * i.e., list->head.next.
 *
 * Preconditions:
 *   - list is valid
 *   - list is non-empty (front requires at least one element)
 *
 * Postconditions:
 *   - returns list->head.next
 *   - list is unchanged (read-only operation)
 *   - list remains valid
 */
void aws_linked_list_front_harness() {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Precondition: list must be non-empty for front() to be valid */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 3. Save old state before calling */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    /* 5. Assert postconditions */

    /* Returns the front element (head.next) */
    assert(result == old_head_next);
    assert(result == list.head.next);

    /* List structure is unchanged (read-only operation) */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    /* Result is not NULL (list was non-empty) */
    assert(result != NULL);

    /* Result is not the tail sentinel (list was non-empty) */
    assert(result != &list.tail);

    /* 6. Assert validity invariant still holds */
    assert(aws_linked_list_is_valid(&list));
}
