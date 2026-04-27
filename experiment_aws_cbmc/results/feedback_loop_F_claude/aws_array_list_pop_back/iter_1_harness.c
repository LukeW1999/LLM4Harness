/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_pop_back(&list);

    /* 4. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: list was non-empty, length decreases by 1 */
        assert(old.length > 0);
        assert(list.length == old.length - 1);
        /* The last element slot is zeroed out */
        /* data pointer, item_size, current_size, alloc unchanged */
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.alloc == old.alloc);
    } else {
        /* Failure: list was empty, nothing changes */
        assert(old.length == 0);
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.alloc == old.alloc);
    }

    /* 5. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
