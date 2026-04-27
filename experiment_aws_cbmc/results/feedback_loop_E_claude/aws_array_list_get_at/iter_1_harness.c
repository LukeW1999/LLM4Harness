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
 * Analysis:
 * 1. Changed fields on success:
 *    - list->data[index * item_size .. (index+1)*item_size] = *val (memcpy)
 *    - list->length may increase to index+1 if index >= old length
 *    - list->current_size may increase (if dynamic and capacity was insufficient)
 *    - list->data pointer may change (if dynamic realloc)
 * 2. Unchanged fields:
 *    - list->item_size
 *    - list->alloc
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

    /* Non-deterministic index */
    size_t index;

    /* Non-deterministic val: must be readable for item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* Save a byte from val to check it wasn't modified */
    struct store_byte_from_buffer val_byte;
    save_byte_from_array(val, list.item_size, &val_byte);

    /* Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size and alloc must never change */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length must be at least index+1 */
        assert(list.length >= index + 1);
        /* The data pointer must be non-null (we wrote something) */
        assert(list.data != NULL);
        /* current_size must be sufficient for the new length */
        assert(list.current_size >= list.length * list.item_size);
        /* val was not modified */
        assert_byte_from_buffer_matches(val, &val_byte);
    } else {
        /* On failure: list must still be valid (already asserted above) */
        /* In static mode, length and current_size should be unchanged */
        /* In dynamic mode, allocation failure may leave list unchanged */
        /* The list validity is the key invariant */
        assert(result == AWS_OP_ERR);
    }
}
