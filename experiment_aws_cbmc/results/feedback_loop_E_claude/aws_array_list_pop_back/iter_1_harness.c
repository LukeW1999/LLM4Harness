/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_array_list_pop_back
 *
 * From the implementation:
 * - If list->length > 0:
 *   - Zeroes out the last element (memset to 0)
 *   - Decrements list->length by 1
 *   - Returns AWS_OP_SUCCESS (0)
 * - If list->length == 0:
 *   - Returns AWS_OP_ERR (-1) with AWS_ERROR_LIST_EMPTY
 *   - list is unchanged
 *
 * Fields that change on success: list->length (decremented by 1), last element zeroed
 * Fields unchanged: list->alloc, list->item_size, list->current_size, list->data pointer
 * On failure: nothing changes
 */
void aws_array_list_pop_back_harness() {
    /* data structure */
    struct aws_array_list list;

    /* bound the list */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* allocate data member */
    ensure_array_list_has_allocated_data_member(&list);

    /* assume valid precondition */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* save old state */
    struct aws_array_list old = list;

    /* save a byte from the data buffer for immutability checks on non-last elements */
    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* save old length for postcondition checks */
    size_t old_length = list.length;

    /* perform operation under verification */
    int result = aws_array_list_pop_back(&list);

    /* assertions */

    /* validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* fields that must never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    if (result == AWS_OP_SUCCESS) {
        /* success path: length was > 0, now decremented by 1 */
        assert(old_length > 0);
        assert(list.length == old_length - 1);
    } else {
        /* failure path: list was empty, nothing changed */
        assert(result == AWS_OP_ERR);
        assert(old_length == 0);
        assert(list.length == old.length);
    }
}
