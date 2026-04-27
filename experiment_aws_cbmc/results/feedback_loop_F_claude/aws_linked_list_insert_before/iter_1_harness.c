/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_before_harness() {
    /* Set up a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* We need a 'before' node that is already in the list.
     * Use the tail sentinel as 'before' to insert at the back,
     * or use head.next if the list is non-empty.
     * For generality, we non-deterministically pick 'before' from
     * the list. The simplest valid choice is list.tail (the sentinel),
     * which is always a valid node in the list. */

    /* Use the tail as the 'before' node (insert before tail = push_back) */
    struct aws_linked_list_node *before = &list.tail;

    /* Create a new node to insert */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* Save old state */
    struct aws_linked_list_node *old_before_prev = before->prev;

    /* Call function under test */
    aws_linked_list_insert_before(before, to_add);

    /* Postconditions from the implementation:
     * to_add->next = before
     * to_add->prev = before->prev (old value)
     * before->prev->next (old before->prev) = to_add
     * before->prev = to_add
     */

    /* 1. Changed fields: to_add's next and prev are set */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);

    /* 2. before->prev now points to to_add */
    assert(before->prev == to_add);

    /* 3. The old predecessor's next now points to to_add */
    assert(old_before_prev->next == to_add);

    /* 4. The list is still valid after insertion */
    assert(aws_linked_list_is_valid(&list));

    /* 5. The list is no longer empty (we inserted a node) */
    assert(!aws_linked_list_empty(&list));

    /* 6. Bidirectional linkage: to_add <-> before */
    assert(aws_linked_list_node_next_is_valid(to_add));

    /* 7. Bidirectional linkage: old_before_prev <-> to_add */
    assert(aws_linked_list_node_prev_is_valid(to_add));
}
