/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <stdlib.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 8
#endif

void aws_array_list_set_at_harness() {
    /* 1. Declare and set up the array list */
    struct aws_array_list list;

    /* Set up item_size first */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    list.item_size = item_size;

    /* Set up length */
    size_t length;
    __CPROVER_assume(length <= MAX_INITIAL_ITEM_ALLOCATION);
    list.length = length;

    /* Set up current_size - must be >= length * item_size */
    size_t current_size;
    __CPROVER_assume(current_size <= MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE);
    __CPROVER_assume(current_size >= length * item_size);
    /* current_size must be a multiple of item_size or zero */
    __CPROVER_assume(item_size == 0 || current_size % item_size == 0);
    list.current_size = current_size;

    /* Set up allocator */
    list.alloc = NULL; /* or some valid allocator */

    /* Set up data pointer */
    if (current_size > 0) {
        list.data = malloc(current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* Verify list is valid before calling */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Non-deterministic val of item_size bytes */
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
