/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index - bound it to avoid huge allocations */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Non-deterministic val of item_size bytes */
    /* item_size must be > 0 for meaningful operation */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 4. Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_alloc = list.alloc;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */
    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size must not change */
    assert(list.item_size == old_item_size);

    /* alloc must not change */
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* The list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* current_size must be >= length * item_size */
        assert(list.current_size >= list.length * list.item_size);

        /* If index was >= old length, length must have increased to index + 1 */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            /* length unchanged if index was within bounds */
            assert(list.length == old_length);
        }

    } else {
        /* On failure, length must not change */
        assert(list.length == old_length);

        /* On failure, current_size must not change */
        assert(list.current_size == old_current_size);
    }
}
