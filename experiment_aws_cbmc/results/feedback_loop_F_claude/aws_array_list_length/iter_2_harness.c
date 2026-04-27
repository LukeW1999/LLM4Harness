/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_length_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old length */
    size_t old_length = list.length;

    /* 3. Call function under test */
    size_t result = aws_array_list_length(&list);

    /* 4. Assert postconditions */

    /* The returned length must equal the list's length field */
    assert(result == list.length);

    /* The length must not have changed */
    assert(list.length == old_length);

    /* The result must equal the old length */
    assert(result == old_length);

    /* The list must still be valid */
    assert(aws_array_list_is_valid(&list));
}
