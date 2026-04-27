/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_ptr_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Output pointer */
    void *val;

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* 5. Call function under test */
    int result = aws_array_list_get_at_ptr(&list, &val, index);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: index < length, val points into the data array at the correct offset */
        assert(list.length > index);
        assert(val == (void *)((uint8_t *)list.data + (list.item_size * index)));
        /* val must be readable (points into allocated data) */
        assert(AWS_MEM_IS_READABLE(val, list.item_size));
    } else {
        /* Failure: index >= length */
        assert(list.length <= index);
    }

    /* 7. Assert unchanged fields regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 8. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
