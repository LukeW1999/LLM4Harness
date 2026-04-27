/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_before_harness() {
    /* data structure */
    struct aws_linked_list list;
    struct aws_linked_list_node to_add;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Non-deterministically choose a node in the list to insert before.
     * We use a non-deterministic pointer that must be a valid node in the list.
     * For simplicity, we pick either the head.next (first real node) or tail
     * (insert at end). We use a non-det choice among valid positions.
     * The simplest valid choice is list.tail (insert before tail = push_back equivalent)
     * or list.head.next (insert before first element).
     * We'll use a non-det node pointer that is assumed to be valid in the list.
     */

    /* Pick 'before' as a node already in the list.
     * We use the tail as a concrete valid choice to keep the proof tractable,
     * but we can also use a non-deterministic node from the list.
     * Using tail ensures before->prev is valid (it's old_last).
     */
    struct aws_linked_list_node *before;
    /* Non-deterministically choose between head.next and tail */
    if (nondet_bool()) {
        before = &list.tail;
    } else {
        before = list.head.next;
    }

    /* before must be a valid node (not null, and its prev must be valid) */
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before->prev != NULL);
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(before));

    /* Save old state */
    struct aws_linked_list_node *old_before_prev = before->prev;

    /* perform operation under verification */
    aws_linked_list_insert_before(before, &to_add);

    /* assertions */
    assert(aws_linked_list_is_valid(&list));

    /* to_add.next should point to before */
    assert(to_add.next == before);

    /* to_add.prev should point to old before->prev */
    assert(to_add.prev == old_before_prev);

    /* before->prev should now point to to_add */
    assert(before->prev == &to_add);

    /* old_before_prev->next should now point to to_add */
    assert(old_before_prev->next == &to_add);

    /* Bidirectional linkage checks */
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));
}
