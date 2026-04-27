/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Non-deterministic value to set (must be readable for item_size bytes) */
    void *val;
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must not change regardless of result */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - The value was copied into the array at index
         * - If index >= old length, length becomes index + 1
         * - current_size may have grown (dynamic mode)
         */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);

        /* Length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was within old bounds, length stays the same */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* length was updated to index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be sufficient to hold the data */
        assert(list.current_size >= list.length * list.item_size);

        /* The value at index matches what was set */
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
        assert_bytes_match(
            (uint8_t *)list.data + (list.item_size * index),
            (uint8_t *)val,
            list.item_size);
    } else {
        /* On failure:
         * - ensure_capacity failed (e.g., allocation failure or static list out of bounds)
         * - The list should remain valid
         * - length and current_size should not have grown beyond what they were
         */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        /* In failure case, list state is preserved (capacity didn't grow) */
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.data == old_list.data);
    }
}
