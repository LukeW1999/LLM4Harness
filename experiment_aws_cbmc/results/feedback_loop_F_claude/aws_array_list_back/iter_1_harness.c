/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate output buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Save old state before calling */
    struct aws_array_list old = list;

    /* Save a byte from the backing data for immutability check */
    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0 && list.data != NULL) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* 4. Call function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: list must have had at least one element */
        assert(old.length > 0);

        /* The val should contain the last element's data */
        size_t last_item_offset = list.item_size * (list.length - 1);
        assert_bytes_match((uint8_t *)val,
                           (uint8_t *)list.data + last_item_offset,
                           list.item_size);
    } else {
        /* Failure: list was empty */
        assert(old.length == 0);
    }

    /* 6. Assert unchanged fields regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 7. Assert backing data is unchanged */
    if (list.current_size > 0 && list.data != NULL) {
        assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
    }

    /* 8. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
