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
    void *val = malloc(list.item_size);
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
        /* On success: the element at index is set, length is updated if index >= old length */
        /* current_size may have grown (dynamic mode) or stayed the same */
        assert(list.current_size >= old_list.current_size);

        /* length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was within old bounds, length stays the same */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* length updated to index + 1 */
            assert(list.length == index + 1);
        }

        /* data pointer must be non-null after successful set */
        assert(list.data != NULL);

        /* The value at index matches what we set */
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
    } else {
        /* On failure: list state should be preserved (capacity may not have changed) */
        /* length should not have increased beyond what it was */
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
    }
}
