/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_front_harness() {
    /* Declare and set up the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Declare the node to push */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Save old state before the call */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    bool was_empty = aws_linked_list_empty(&list);

    /* Call function under test */
    aws_linked_list_push_front(&list, node);

    /* Postcondition 1: list is still valid */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 2: node is now the first element (head.next == node) */
    assert(list.head.next == node);

    /* Postcondition 3: node's prev points to head */
    assert(node->prev == &list.head);

    /* Postcondition 4: node's next points to what was previously the first element */
    assert(node->next == old_head_next);

    /* Postcondition 5: the old first element's prev now points to node */
    assert(old_head_next->prev == node);

    /* Postcondition 6: head.prev is still NULL (unchanged) */
    assert(list.head.prev == NULL);

    /* Postcondition 7: tail.next is still NULL (unchanged) */
    assert(list.tail.next == NULL);

    /* Postcondition 8: if the list was empty before, tail.prev now points to node */
    if (was_empty) {
        assert(list.tail.prev == node);
    } else {
        /* tail.prev is unchanged when list was non-empty */
        assert(list.tail.prev == old_tail_prev);
    }

    /* Postcondition 9: list is not empty after push_front */
    assert(!aws_linked_list_empty(&list));

    /* Postcondition 10: node linkage is bidirectional */
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(node));
}
