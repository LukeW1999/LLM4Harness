/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_begin_harness() {
    /* data structure */
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list_node *old_head_next = list.head.next;

    /* perform operation under verification */
    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    /* assertions */
    /* 1. Changed fields: result is the first element (head.next) */
    assert(result == old_head_next);
    assert(result == list.head.next);

    /* 2. Unchanged fields: the list structure itself is not modified */
    assert(list.head.next == old_head_next);

    /* 3. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
}
