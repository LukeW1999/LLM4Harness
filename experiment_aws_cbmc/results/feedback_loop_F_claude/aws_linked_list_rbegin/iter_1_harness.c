/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_linked_list_rbegin
 *
 * aws_linked_list_rbegin returns list->tail.prev, which is the last element
 * in the list (or head if the list is empty).
 *
 * 1. Changed fields: none — this is a read-only accessor
 * 2. Unchanged fields: all list fields remain unchanged
 * 3. Failure: no failure path (always returns list->tail.prev)
 * 4. Validity invariants: list remains valid after the call
 */
void aws_linked_list_rbegin_harness() {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    /* 3. Call function under test */
    struct aws_linked_list_node *rval = aws_linked_list_rbegin(&list);

    /* 4. Assert postconditions */

    /* The return value must be list->tail.prev */
    assert(rval == old_tail_prev);
    assert(rval == list.tail.prev);

    /* All list fields must remain unchanged (read-only accessor) */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    /* Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
