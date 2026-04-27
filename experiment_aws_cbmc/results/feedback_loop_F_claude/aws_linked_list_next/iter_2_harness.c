/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_next_harness() {
    /* Set up linked list nodes */
    struct aws_linked_list_node node;
    struct aws_linked_list_node next_node;

    /* node.next points to next_node */
    node.next = &next_node;
    node.prev = &next_node; /* prev must be valid too for node_is_valid */

    /* next_node needs valid next and prev pointers */
    next_node.next = &node;
    next_node.prev = &node;

    /* Assume preconditions: node->next must be valid */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node));
    /* The result (next_node) must have valid prev */
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&next_node));

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
