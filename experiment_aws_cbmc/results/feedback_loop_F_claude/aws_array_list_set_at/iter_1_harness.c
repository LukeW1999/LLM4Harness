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

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Non-deterministic val of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* The list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* item_size must not change */
        assert(list.item_size == old_list.item_size);

        /* alloc must not change */
        assert(list.alloc == old_list.alloc);

        /* The data at index must match val */
        assert(memcmp((uint8_t *)list.data + (list.item_size * index), val, list.item_size) == 0);

        /* If index was >= old length, length must have increased to index + 1 */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            /* length unchanged if index was within bounds */
            assert(list.length == old_list.length);
        }

        /* current_size must be >= length * item_size */
        assert(list.current_size >= list.length * list.item_size);

    } else {
        /* On failure, the list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* On failure, item_size must not change */
        assert(list.item_size == old_list.item_size);

        /* On failure, alloc must not change */
        assert(list.alloc == old_list.alloc);

        /* On failure, length must not change */
        assert(list.length == old_list.length);

        /* On failure, current_size must not change */
        assert(list.current_size == old_list.current_size);
    }

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
