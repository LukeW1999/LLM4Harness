/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_end_harness() {
    /* data structure */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* perform operation under verification */
    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    /* postconditions */
    /* The function returns a pointer to the tail node of the list */
    assert(result == &list.tail);

    /* The list remains valid after the call */
    assert(aws_linked_list_is_valid(&list));
}
