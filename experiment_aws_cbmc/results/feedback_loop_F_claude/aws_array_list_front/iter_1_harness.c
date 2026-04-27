/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministic index */
    size_t index;

    /* Non-deterministic value to set */
    /* val must be readable for item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 2. Save old state BEFORE calling */
    struct aws_array_list old_list = list;

    /* 3. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 4. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: the list is valid */
        assert(aws_array_list_is_valid(&list));

        /* The item_size must not change */
        assert(list.item_size == old_list.item_size);

        /* The allocator must not change */
        assert(list.alloc == old_list.alloc);

        /* The length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was already within bounds, length is max(old_length, index+1) */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_list.length);
        }

        /* The data at the index must match val */
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
        uint8_t *stored = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(stored, val, list.item_size);

        /* current_size must be >= length * item_size */
        size_t required_size = list.length * list.item_size;
        assert(list.current_size >= required_size);

    } else {
        /* On failure: the list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* item_size unchanged */
        assert(list.item_size == old_list.item_size);

        /* allocator unchanged */
        assert(list.alloc == old_list.alloc);

        /* length unchanged on failure */
        assert(list.length == old_list.length);

        /* current_size unchanged on failure */
        assert(list.current_size == old_list.current_size);
    }

    /* 5. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
