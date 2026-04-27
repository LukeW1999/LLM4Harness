/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    /* Declare and set up the array list */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Allocate the data member non-deterministically */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;

    /* Non-deterministic index */
    size_t index;

    /* Allocate val with item_size bytes of readable memory */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save a byte from val for immutability check */
    struct store_byte_from_buffer old_val_byte;
    save_byte_from_array(val, list.item_size, &old_val_byte);

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Assert unchanged fields regardless of result */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);

    /* val must not have been modified */
    assert_byte_from_buffer_matches(val, &old_val_byte);

    if (result == AWS_OP_SUCCESS) {
        /* On success the list must have data */
        assert(list.data != NULL);

        /* Length update: if index was >= old length, length = index + 1 */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }

        /* current_size must be at least enough to hold index+1 elements */
        assert(list.current_size >= (index + 1) * list.item_size);

        /* The data at index should match val */
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), val, list.item_size);

    } else {
        /* On failure, length should not have changed */
        assert(list.length == old_length);
    }
}
