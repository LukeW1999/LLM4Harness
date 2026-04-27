/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_remove_harness() {
    /* data structure */
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Pick a non-deterministic node from the list to remove.
     * We use the first node (head.next) as a representative node in the list.
     * Since the list is non-empty, head.next != &list.tail. */
    struct aws_linked_list_node *node = list.head.next;
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node != &list.tail);
    __CPROVER_assume(node->prev != NULL);
    __CPROVER_assume(node->next != NULL);

    /* Save the neighbors before removal */
    struct aws_linked_list_node *prev_node = node->prev;
    struct aws_linked_list_node *next_node = node->next;

    /* perform operation under verification */
    aws_linked_list_remove(node);

    /* assertions */

    /* 1. The list remains valid after removal */
    assert(aws_linked_list_is_valid(&list));

    /* 2. The prev and next of the removed node are now NULL (reset) */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* 3. The neighbors are now linked to each other */
    assert(prev_node->next == next_node);
    assert(next_node->prev == prev_node);

    /* 4. The neighbors' connections are bidirectional */
    assert(aws_linked_list_node_next_is_valid(prev_node));
    assert(aws_linked_list_node_prev_is_valid(next_node));
}
