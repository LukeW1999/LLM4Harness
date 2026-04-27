/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_array_list_pop_front
 *
 * From Doxygen:
 * "Deletes the element at the front of the list if it exists. If list is empty,
 *  AWS_ERROR_LIST_EMPTY will be raised. This call results in shifting all of the
 *  elements at the end of the array to the front."
 *
 * Analysis:
 * 1. Changed on success: list.length decreases by 1 (element removed from front)
 * 2. Unchanged: list.alloc, list.item_size, list.current_size, list.data pointer
 * 3. On failure (empty list): list is unchanged, error is raised
 * 4. Validity invariant: aws_array_list_is_valid must hold after call
 */
void aws_array_list_pop_front_harness() {
    /* data structure */
    struct aws_array_list list;

    /* assumptions: bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* save old state before calling */
    struct aws_array_list old = list;

    /* perform operation under verification */
    int result = aws_array_list_pop_front(&list);

    /* assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* success: list was non-empty, length decreased by 1 */
        assert(old.length > 0);
        assert(list.length == old.length - 1);
    } else {
        /* failure: list was empty, nothing changed */
        assert(old.length == 0);
        assert(list.length == old.length);
    }

    /* fields that must not change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
