/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_push_back_harness() {
    /* data structure */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* Non-deterministic val: must be readable for item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(list.item_size > 0);

    /* Save a byte from val to check it wasn't modified */
    struct store_byte_from_buffer old_val_byte;
    save_byte_from_array(val, list.item_size, &old_val_byte);

    /* Call function under test */
    int result = aws_array_list_push_back(&list, val);

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size and alloc never change */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length must increase by 1 */
        assert(list.length == old.length + 1);

        /* current_size must be large enough to hold the data */
        assert(list.current_size >= list.length * list.item_size);

        /* data pointer must be non-null */
        assert(list.data != NULL);

        /* val was not modified */
        assert_byte_from_buffer_matches(val, &old_val_byte);
    } else {
        /* On failure: list remains valid (already asserted above) */
        /* length should not change on failure */
        assert(list.length == old.length);
    }
}
