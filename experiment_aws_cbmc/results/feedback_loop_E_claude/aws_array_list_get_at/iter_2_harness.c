/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_array_list_get_at
 */
void aws_array_list_get_at_harness() {
    /* data structure */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old_list = list;
    struct store_byte_from_buffer old_byte;
    if (list.data && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* Non-deterministic index */
    size_t index;

    /* Output buffer: must be writable for item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Call function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size and alloc must never change */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    /* length and current_size must never change (get_at is read-only) */
    assert(list.length == old_list.length);
    assert(list.current_size == old_list.current_size);

    if (result == AWS_OP_SUCCESS) {
        /* On success: index must be within bounds */
        assert(index < list.length);
        /* val must have been written (data is non-null) */
        assert(list.data != NULL);
    } else {
        /* On failure: index must be out of bounds */
        assert(result == AWS_OP_ERR);
        assert(index >= list.length);
    }
}
