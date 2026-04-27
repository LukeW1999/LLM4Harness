/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_back_harness() {
    /* data structure */
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Precondition: list must be valid and non-empty (back requires non-empty list) */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save the expected back node before the call */
    struct aws_linked_list_node *expected_back = list.tail.prev;

    /* perform operation under verification */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* assertions */
    /* 1. Changed fields: result should be the last element (tail.prev) */
    assert(result == expected_back);
    assert(result == list.tail.prev);

    /* 2. Unchanged fields: the list structure itself should not be modified */
    assert(aws_linked_list_is_valid(&list));

    /* 3. The result node should have valid linkage */
    assert(result != NULL);
    assert(aws_linked_list_node_prev_is_valid(result));

    /* 4. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
    assert(!aws_linked_list_empty(&list));
}
