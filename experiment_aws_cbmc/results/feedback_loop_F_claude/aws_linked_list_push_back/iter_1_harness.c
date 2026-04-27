/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_back_harness() {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Declare the node to push back */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* 3. Save old state before calling */
    /* Save the old last node (the node that was previously before tail) */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* 4. Call function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Assert postconditions */

    /* Changed fields: node is now the last element (just before tail) */
    assert(list.tail.prev == node);

    /* The new node's next should point to tail */
    assert(node->next == &list.tail);

    /* The new node's prev should point to what was previously the last node */
    assert(node->prev == old_tail_prev);

    /* The old last node's next should now point to the new node */
    assert(old_tail_prev->next == node);

    /* Sentinel structure: head.prev == NULL, tail.next == NULL */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* head.next->prev == &list.head (head sentinel linkage) */
    assert(list.head.next->prev == &list.head);

    /* node linkage validity */
    assert(node->next->prev == node);  /* aws_linked_list_node_next_is_valid(node) */
    assert(node->prev->next == node);  /* aws_linked_list_node_prev_is_valid(node) */

    /* tail linkage validity */
    assert(list.tail.prev->next == &list.tail);  /* aws_linked_list_node_prev_is_valid(&list.tail) */

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
}
