/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_next_harness() {
    /* Set up a linked list node with a non-deterministic next pointer */
    struct aws_linked_list_node node;
    struct aws_linked_list_node next_node;

    /* node.next points to next_node (non-deterministically could be anything valid) */
    node.next = &next_node;
    node.prev = NULL; /* prev not relevant for this function */

    /* Save old state */
    struct aws_linked_list_node *old_next = node.next;

    /* Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    /* Postconditions:
     * 1. The function returns node->next
     * 2. node itself is unchanged
     */
    assert(result == old_next);
    assert(result == node.next);
    assert(node.next == old_next);
}
