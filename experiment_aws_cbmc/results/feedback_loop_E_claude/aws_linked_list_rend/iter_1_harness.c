/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_rend_harness() {
    /* data structure */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* perform operation under verification */
    const struct aws_linked_list_node *rend = aws_linked_list_rend(&list);

    /* assertions */
    /* The function returns a pointer to the head of the list (one before the first element) */
    assert(rend == &list.head);

    /* The list must remain valid after the call */
    assert(aws_linked_list_is_valid(&list));
}
