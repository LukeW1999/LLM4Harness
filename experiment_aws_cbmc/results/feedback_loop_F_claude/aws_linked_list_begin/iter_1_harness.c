/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_linked_list_begin
 *
 * The function returns an iteration pointer for the first element in the list,
 * which is list->head.next.
 *
 * 1. Changed fields: none (read-only operation)
 * 2. Unchanged fields: entire list structure is unchanged
 * 3. Failure: no failure path (always returns head.next)
 * 4. Validity invariants: list remains valid after the call
 */
void aws_linked_list_begin_harness() {
    /* Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state before calling */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    /* Assert postconditions */

    /* The function returns list->head.next (the first element) */
    assert(result == list.head.next);
    assert(result == old_head_next);

    /* The list structure must not be modified (read-only operation) */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    /* Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
