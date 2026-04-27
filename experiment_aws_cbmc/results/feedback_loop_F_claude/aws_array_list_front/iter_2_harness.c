/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_front_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Allocate output buffer for the front element */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save old state BEFORE calling */
    struct aws_array_list old_list = list;

    /* Call function under test */
    int result = aws_array_list_front(&list, val);

    /* Assert postconditions */
    /* The list must always remain valid */
    assert(aws_array_list_is_valid(&list));

    /* item_size must not change */
    assert(list.item_size == old_list.item_size);

    /* allocator must not change */
    assert(list.alloc == old_list.alloc);

    /* length must not change */
    assert(list.length == old_list.length);

    /* current_size must not change */
    assert(list.current_size == old_list.current_size);

    /* data pointer must not change */
    assert(list.data == old_list.data);

    if (result == AWS_OP_SUCCESS) {
        /* On success: list must have had at least one element */
        assert(old_list.length > 0);

        /* val must be readable */
        assert(AWS_MEM_IS_READABLE(val, list.item_size));

        /* The data at index 0 must match val */
        if (list.item_size > 0 && list.data != NULL) {
            uint8_t *stored = (uint8_t *)list.data;
            assert_bytes_match(stored, val, list.item_size);
        }
    } else {
        /* On failure: list must have been empty */
        assert(old_list.length == 0);
    }
}
