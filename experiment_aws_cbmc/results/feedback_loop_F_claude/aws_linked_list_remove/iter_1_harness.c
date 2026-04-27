/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_linked_list_remove.
 *
 * The function removes a node from the list:
 *   - node->prev->next = node->next
 *   - node->next->prev = node->prev
 *   - node->next = NULL, node->prev = NULL (via aws_linked_list_node_reset)
 *
 * Preconditions:
 *   - The list must be valid (deep)
 *   - The node must be in the list (not the head or tail sentinel)
 *
 * Postconditions:
 *   - node->next == NULL
 *   - node->prev == NULL
 *   - The list remains valid
 *   - The neighbors of the removed node now point to each other
 */
void aws_linked_list_remove_harness() {
    /* Set up a non-deterministic linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* The list must be non-empty to remove a node */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Pick a non-deterministic node from the list that is not head or tail */
    /* We'll use the first node (head.next) as a concrete non-sentinel node */
    struct aws_linked_list_node *node = list.head.next;

    /* Ensure the node is a valid interior node (not the tail sentinel) */
    __CPROVER_assume(node != &list.tail);
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node->next != NULL);
    __CPROVER_assume(node->prev != NULL);

    /* Save the neighbors before removal */
    struct aws_linked_list_node *prev_node = node->prev;
    struct aws_linked_list_node *next_node = node->next;

    /* Precondition: the node's connections are bidirectional */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));

    /* Call the function under test */
    aws_linked_list_remove(node);

    /* Postcondition 1: node's pointers are reset to NULL */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* Postcondition 2: neighbors now point to each other */
    assert(prev_node->next == next_node);
    assert(next_node->prev == prev_node);

    /* Postcondition 3: the list remains valid */
    assert(aws_linked_list_is_valid(&list));
}
