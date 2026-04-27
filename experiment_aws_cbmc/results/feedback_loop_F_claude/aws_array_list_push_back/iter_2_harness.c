/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_array_list_push_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic value to push (must be readable for item_size bytes) */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Save old state */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;

    /* 4. Call function under test */
    int result = aws_array_list_push_back(&list, val);

    /* 5. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must not change regardless of result */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length increases by 1 */
        assert(list.length == old_length + 1);

        /* current_size must be at least as large */
        assert(list.current_size >= old_current_size);

        /* data pointer must be non-null after successful push */
        assert(list.data != NULL);

        /* current_size must accommodate the new length */
        assert(list.current_size >= list.length * list.item_size);
    } else {
        /* On failure: list state should be preserved */
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
        assert(list.data == old_list.data);
    }
}
