/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_linked_list_prev
 *
 * The function returns node->prev.
 *
 * 1. Changed fields: none (pure accessor)
 * 2. Unchanged fields: node->next, node->prev (not modified)
 * 3. Failure: no failure path (always returns node->prev)
 * 4. Validity invariants: node->prev must be non-null (assumed valid list node)
 */
void aws_linked_list_prev_harness() {
    /* Set up a linked list with allocated nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* We need a node that is in the list to call prev on.
     * Use the tail sentinel as the node to call prev on,
     * since tail always has a valid prev pointer (either head or a real node).
     * Alternatively, use a non-deterministic node from the list.
     * We'll use list.tail since it always has a valid prev. */
    struct aws_linked_list_node *node = &list.tail;

    /* Precondition: node is non-null and node->prev is non-null */
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node->prev != NULL);

    /* Save old state */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* Postconditions */
    /* 1. Return value equals node->prev */
    assert(result == old_prev);

    /* 2. Node fields are unchanged */
    assert(node->prev == old_prev);
    assert(node->next == old_next);

    /* 3. List validity is preserved */
    assert(aws_linked_list_is_valid(&list));
}
