/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_array_list_clear
 *
 * From the API:
 * "Clears all elements in the array. Does not free any memory."
 *
 * Analysis:
 * 1. Changed on success: list->length becomes 0
 * 2. Unchanged: list->item_size, list->alloc, list->current_size, list->data
 * 3. Validity invariant: aws_array_list_is_valid must hold after call
 */
void aws_array_list_clear_harness() {
    /* data structure */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old_list = list;

    /* perform operation under verification */
    aws_array_list_clear(&list);

    /* assertions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* length must be 0 after clear */
    assert(list.length == 0);

    /* item_size, alloc, current_size, and data pointer never change */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.data == old_list.data);
}
