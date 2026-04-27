/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_array_list_length
 */
void aws_array_list_length_harness() {
    /* data structure */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    size_t old_length = list.length;

    /* perform operation under verification */
    size_t length = aws_array_list_length(&list);

    /* assertions */
    /* The returned length must equal the list's length field */
    assert(length == old_length);

    /* The list must still be valid after the call */
    assert(aws_array_list_is_valid(&list));

    /* The list must not have been modified */
    assert(list.length == old_length);
}
