/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_array_list_capacity
 */
void aws_array_list_capacity_harness() {
    /* data structure */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Call function under test */
    size_t capacity = aws_array_list_capacity(&list);

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* The capacity should equal current_size / item_size when item_size > 0 */
    if (list.item_size > 0) {
        assert(capacity == list.current_size / list.item_size);
    } else {
        assert(capacity == 0);
    }
}
