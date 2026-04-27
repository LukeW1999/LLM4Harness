/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_prev_harness() {
    /* data structure */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Choose a non-deterministic node to call prev on.
     * We need a node that has a valid prev pointer.
     * We use the tail sentinel node, which always has a valid prev pointer
     * (pointing to the last element or to head if empty).
     * Alternatively, we can use any node in the list.
     * For generality, we use the tail node since it always has a prev. */
    struct aws_linked_list_node *node = &list.tail;

    /* Save the expected prev pointer before the call */
    struct aws_linked_list_node *expected_prev = node->prev;

    /* perform operation under verification */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* assertions */
    /* The function returns node->prev */
    assert(result == expected_prev);

    /* The node itself is unchanged */
    assert(node->prev == expected_prev);

    /* The list remains valid */
    assert(aws_linked_list_is_valid(&list));
}
