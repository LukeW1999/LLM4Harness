/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_array_list_front
 *
 * Gets the first element of the list.
 * On success: copies the first element into *val, returns AWS_OP_SUCCESS
 * On failure: returns AWS_OP_ERR (list is empty)
 */
void aws_array_list_front_harness() {
    /* data structure */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old_list = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* Allocate output buffer for the front element */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Call function under test */
    int result = aws_array_list_front(&list, val);

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Unchanged fields regardless of result */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);

    /* List data should not have been modified */
    if (list.current_size > 0) {
        assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
    }

    if (result == AWS_OP_SUCCESS) {
        /* On success, list must have had at least one element */
        assert(old_list.length > 0);
        assert(list.data != NULL);

        /* The value copied out should match the first element */
        assert_bytes_match((uint8_t *)val, (uint8_t *)list.data, list.item_size);
    } else {
        /* On failure, list must have been empty */
        assert(result == AWS_OP_ERR);
        assert(old_list.length == 0);
    }
}
