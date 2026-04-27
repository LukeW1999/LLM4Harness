/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>

void aws_array_list_erase_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;

    /* Save a byte from the data for checking immutability on failure */
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: index must have been valid (< length) */
        assert(index < old_length);
        /* Length decreases by 1 */
        assert(list.length == old_length - 1);
        /* item_size, alloc, current_size, data pointer unchanged */
        assert(list.item_size == old_item_size);
        assert(list.alloc == old_alloc);
        assert(list.current_size == old_current_size);
        assert(list.data == old_data);
    } else {
        /* On failure: index must have been out of bounds */
        assert(index >= old_length);
        /* List is unchanged */
        assert(list.length == old_length);
        assert(list.item_size == old_item_size);
        assert(list.alloc == old_alloc);
        assert(list.current_size == old_current_size);
        assert(list.data == old_data);
        /* Data content unchanged */
        if (old_current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }

    /* 6. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
