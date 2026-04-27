/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_front_harness() {
    /* data structure */
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Precondition: list must be valid and non-empty */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Keep the old front node and the node after it */
    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_second = list.head.next->next;

    /* perform operation under verification */
    struct aws_linked_list_node *front = aws_linked_list_pop_front(&list);

    /* assertions */
    /* The returned node is the old front */
    assert(front == old_front);

    /* The returned node's next and prev are NULL (reset) */
    assert(front->next == NULL);
    assert(front->prev == NULL);

    /* The list is still valid */
    assert(aws_linked_list_is_valid(&list));

    /* The new front of the list is the old second node */
    assert(list.head.next == old_second);

    /* The new front's prev points to head */
    assert(old_second->prev == &list.head);

    /* head's next points to old_second */
    assert(list.head.next == old_second);

    /* head's prev is still NULL */
    assert(list.head.prev == NULL);

    /* tail's next is still NULL */
    assert(list.tail.next == NULL);
}
