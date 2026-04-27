/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_next_harness() {
    /* data structure */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Pick a node from the list to call next on.
     * We use the head node as a valid node with a non-NULL next pointer. */
    struct aws_linked_list_node *node = aws_linked_list_begin(&list);

    /* Save the expected next pointer before the call */
    struct aws_linked_list_node *expected_next = node->next;

    /* perform operation under verification */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* assertions */
    /* The return value must equal node->next */
    assert(result == expected_next);

    /* The node itself must not have been modified */
    assert(node->next == expected_next);

    /* The list must remain valid */
    assert(aws_linked_list_is_valid(&list));
}
