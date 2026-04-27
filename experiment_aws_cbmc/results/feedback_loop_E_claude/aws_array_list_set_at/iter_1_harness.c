/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_array_list_set_at
 *
 * Analysis:
 * 1. Changed fields on success:
 *    - list->data[index * item_size .. (index+1)*item_size] = *val (memcpy)
 *    - list->length = index + 1 if index >= old length
 *    - list->current_size may grow (via ensure_capacity)
 * 2. Unchanged fields:
 *    - list->alloc
 *    - list->item_size
 * 3. On failure: returns AWS_OP_ERR (ensure_capacity failed or overflow)
 * 4. Validity invariant: aws_array_list_is_valid must hold after call
 */
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
    struct aws_array_list old_list = list;

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
    assert(list.alloc == old_list.alloc);
    assert(list.item_size == old_list.item_size);

    /* val must not have been modified */
    assert_byte_from_buffer_matches(val, &old_val_byte);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - The element at index was set to val
         * - If index >= old length, length = index + 1
         * - current_size may have grown or stayed the same
         */
        assert(list.data != NULL);

        /* The data at index should match val */
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), val, list.item_size);

        /* Length update: if index was >= old length, length = index + 1 */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_list.length);
        }

        /* current_size must be at least enough to hold index+1 elements */
        assert(list.current_size >= (index + 1) * list.item_size);

    } else {
        /* On failure:
         * - ensure_capacity failed or overflow in aws_add_size_checked
         * - The list state may be partially modified (capacity could have changed)
         *   but length should not have increased beyond what it was
         * - item_size and alloc are unchanged (already asserted above)
         */
        /* Length should not have changed on failure paths */
        /* Note: ensure_capacity failure happens before memcpy, so data is unchanged */
        /* The length update only happens after memcpy succeeds */
        assert(list.length == old_list.length);
    }
}
