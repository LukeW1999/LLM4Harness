/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_node_reset_harness() {
    /* data structure */
    struct aws_linked_list_node node;

    /* node must be non-NULL (precondition: node != NULL) */
    /* node is allocated on the stack, so it's non-NULL by definition */

    /* perform operation under verification */
    aws_linked_list_node_reset(&node);

    /* assertions */
    /* Postcondition: node's next and prev pointers are set to NULL */
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
