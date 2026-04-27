/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_array_list_set_at
 *
 * From Doxygen:
 * "Copies the memory pointed to by val into the array at index. If in dynamic mode,
 *  the size will grow by a factor of two when the array is full. In static mode,
 *  AWS_ERROR_INVALID_INDEX will be raised if the index is past the bounds of the array."
 *
 * Analysis:
 * 1. Changed on success: list->length may increase (if index >= old length),
 *    list->data contents at index are updated, list->current_size may grow (dynamic).
 * 2. Unchanged: list->item_size, list->alloc
 * 3. On failure: list remains valid, returns AWS_OP_ERR
 * 4. Validity invariant: aws_array_list_is_valid must hold after call
 */
void aws_array_list_set_at_harness() {
    /* data structure */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old_list = list;

    /* parameters */
    size_t index;
    /* Bound index to keep state space manageable */
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* val must be readable for item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save a byte from val to check it wasn't modified */
    struct store_byte_from_buffer val_byte;
    save_byte_from_array(val, list.item_size, &val_byte);

    /* perform operation under verification */
    int result = aws_array_list_set_at(&list, val, index);

    /* assertions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size and alloc never change */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: if index >= old length, length becomes index + 1 */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            /* length unchanged if index was within bounds */
            assert(list.length == old_list.length);
        }
        /* current_size must be at least enough to hold the new length */
        assert(list.current_size >= list.length * list.item_size);
        /* data pointer must be non-null after successful set */
        assert(list.data != NULL);
    } else {
        /* On failure: length and current_size should be unchanged */
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
    }

    /* val should not have been modified by the function */
    assert_byte_from_buffer_matches(val, &val_byte);
}
