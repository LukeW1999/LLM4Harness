/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 8
#endif

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;

    /* Bound the list to keep verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    /* 2. Create a non-deterministic value to push */
    /* Use a fixed-size buffer to avoid issues with malloc returning NULL */
    uint8_t val_buf[MAX_ITEM_SIZE];
    void *val = (void *)val_buf;

    /* 3. Save old state before calling */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 4. Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Assert postconditions - these must be reachable */
    /* item_size and alloc should never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length increases by 1 */
        assert(list.length == old_length + 1);
        assert(list.data != NULL);
        assert(list.current_size >= list.length * list.item_size);
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure: length unchanged */
        assert(list.length == old_length);
        assert(aws_array_list_is_valid(&list));
    }

    /* 6. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
