/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_rbegin_harness() {
    /* data structure */
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* perform operation under verification */
    struct aws_linked_list_node *rval = aws_linked_list_rbegin(&list);

    /* assertions */
    /* 1. Changed fields: rval should be list.tail.prev */
    assert(rval == old_tail_prev);
    assert(rval == list.tail.prev);

    /* 2. Unchanged fields: the list itself should not be modified */
    assert(list.head.next != NULL);
    assert(list.tail.prev != NULL);

    /* 3. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
}
