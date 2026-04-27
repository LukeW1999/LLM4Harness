/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_front_harness() {
    /* data structure */
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Precondition: list must be valid and non-empty (front requires at least one element) */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save the expected front node before the call */
    struct aws_linked_list_node *expected_front = list.head.next;

    /* perform operation under verification */
    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    /* assertions */
    /* The function returns the element at the front of the list (head.next) */
    assert(result == expected_front);
    assert(result == list.head.next);

    /* The list structure must remain unchanged */
    assert(aws_linked_list_is_valid(&list));

    /* head and tail pointers must not have changed */
    assert(list.head.next == expected_front);

    /* The result node's linkage must be valid */
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));
}
