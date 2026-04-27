/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_linked_list_node_reset
 *
 * Doxygen: "Set node's next and prev pointers to NULL."
 *
 * Analysis:
 * 1. Changed fields on success: node->next == NULL, node->prev == NULL
 * 2. Unchanged fields: none (the entire node is zeroed)
 * 3. Failure: no failure path (precondition: node != NULL)
 * 4. Validity invariants: node->next == NULL && node->prev == NULL (AWS_IS_ZEROED)
 */
void aws_linked_list_node_reset_harness() {
    /* Declare the node under test */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Non-deterministically initialize the node's fields */
    node->next = nondet_bool() ? NULL : malloc(sizeof(struct aws_linked_list_node));
    node->prev = nondet_bool() ? NULL : malloc(sizeof(struct aws_linked_list_node));

    /* Call the function under test */
    aws_linked_list_node_reset(node);

    /* Postconditions: node->next and node->prev must be NULL */
    assert(node->next == NULL);
    assert(node->prev == NULL);
}
