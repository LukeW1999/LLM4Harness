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
 * the size will grow by a factor of two when the array is full. In static mode,
 * AWS_ERROR_INVALID_INDEX will be raised if the index is past the bounds of the array."
 *
 * On success:
 *   - The element at index is set to val
 *   - list->length may increase (if index >= old length, length = index + 1)
 *   - list remains valid
 *
 * On failure:
 *   - list remains valid
 *   - Returns AWS_OP_ERR
 *
 * Unchanged fields:
 *   - list->item_size
 *   - list->alloc
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

    /* Unchanged fields regardless of result */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    /* val should not have been modified */
    assert_byte_from_buffer_matches(val, &old_val_byte);

    if (result == AWS_OP_SUCCESS) {
        /* On success, the element at index is set to val */
        /* length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was within old bounds, length stays the same or grows */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* length = index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be sufficient */
        assert(list.current_size >= list.length * list.item_size);

        /* data pointer must be non-null */
        assert(list.data != NULL);

        /* The value at index should match val */
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), val, list.item_size);
    } else {
        /* On failure, list should remain valid (already asserted above) */
        /* The list state may be unchanged or partially changed, but must be valid */
        assert(result == AWS_OP_ERR);
    }
}
