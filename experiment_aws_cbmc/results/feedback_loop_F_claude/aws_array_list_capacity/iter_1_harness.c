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
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Non-deterministic value to set */
    /* val must be readable for item_size bytes */
    uint8_t val_buf[MAX_ITEM_SIZE];
    void *val = (void *)val_buf;
    __CPROVER_assume(list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE);

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - The item at index is set (memcpy'd from val)
         * - If index >= old length, length becomes index + 1
         * - item_size unchanged
         * - alloc unchanged
         */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);

        /* Length: if index was >= old length, new length = index + 1 */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_list.length);
        }

        /* current_size can only grow or stay the same */
        assert(list.current_size >= old_list.current_size);

        /* The data pointer must be non-null since we successfully set an element */
        assert(list.data != NULL);

        /* The value at index matches what we wrote */
        assert(AWS_MEM_IS_WRITABLE(list.data, list.current_size));
        assert(memcmp((uint8_t *)list.data + (list.item_size * index), val, list.item_size) == 0);

    } else {
        /* On failure:
         * - The list should remain valid
         * - item_size unchanged
         * - alloc unchanged
         * - length unchanged (capacity expansion failed)
         */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
    }
}
