/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_front_harness() {
    /* Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Precondition: list must not be empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save old state before calling */
    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_second = list.head.next->next;

    /* Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    /* Postcondition 1: The returned node is the old front node */
    assert(result == old_front);

    /* Postcondition 2: The returned node's next and prev are NULL (reset) */
    assert(result->next == NULL);
    assert(result->prev == NULL);

    /* Postcondition 3: The list is still valid after the operation */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 4: The new front of the list is what was previously second */
    assert(list.head.next == old_second);

    /* Postcondition 5: The new front's prev points to head */
    assert(list.head.next->prev == &list.head);

    /* Postcondition 6: head.prev is still NULL (unchanged) */
    assert(list.head.prev == NULL);

    /* Postcondition 7: tail.next is still NULL (unchanged) */
    assert(list.tail.next == NULL);
}
