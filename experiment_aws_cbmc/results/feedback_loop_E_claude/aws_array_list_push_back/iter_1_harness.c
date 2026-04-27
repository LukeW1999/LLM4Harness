/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_array_list_set_at
 *
 * Doxygen: Copies the memory pointed to by val into the array at index.
 * If in dynamic mode, the size will grow by a factor of two when the array is full.
 * In static mode, AWS_ERROR_INVALID_INDEX will be raised if the index is past the bounds.
 *
 * Fields that change on success:
 *   - list->data[index * item_size .. (index+1)*item_size] = *val
 *   - list->length may increase to index+1 if index >= old length
 *   - list->current_size may increase (dynamic mode realloc)
 *   - list->data pointer may change (dynamic mode realloc)
 *
 * Fields unchanged:
 *   - list->item_size
 *   - list->alloc
 *
 * On failure:
 *   - list remains valid
 *   - returns AWS_OP_ERR
 *
 * Validity invariant: aws_array_list_is_valid(list) always holds after call.
 */
void aws_array_list_set_at_harness() {
    /* data structure */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* Non-deterministic index */
    size_t index;

    /* Non-deterministic val: must be readable for item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save a byte from val to check it wasn't modified */
    struct store_byte_from_buffer old_val_byte;
    save_byte_from_array(val, list.item_size, &old_val_byte);

    /* Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size and alloc never change */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length must be at least index+1 */
        assert(list.length >= index + 1);

        /* If index was already within bounds, length doesn't shrink */
        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            /* length updated to index+1 */
            assert(list.length == index + 1);
        }

        /* current_size must be large enough to hold the data */
        assert(list.current_size >= list.length * list.item_size);

        /* data pointer must be non-null */
        assert(list.data != NULL);

        /* val was not modified */
        assert_byte_from_buffer_matches(val, &old_val_byte);
    } else {
        /* On failure: list remains valid (already asserted above) */
        /* In static mode: length and current_size unchanged */
        if (list.alloc == NULL) {
            assert(list.length == old.length);
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        }
    }
}
