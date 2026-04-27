/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_push_front_harness() {
    /* data structure */
    struct aws_array_list list;

    /* assumptions: bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* val: a readable buffer of item_size bytes */
    __CPROVER_assume(list.item_size > 0);
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* save old state */
    struct aws_array_list old_list = list;

    /* save a byte from the val buffer to check it's not modified */
    struct store_byte_from_buffer old_val_byte;
    save_byte_from_array(val, list.item_size, &old_val_byte);

    /* perform operation under verification */
    int result = aws_array_list_push_front(&list, val);

    /* assertions */

    /* validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* item_size and alloc must never change */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length increases by 1 */
        assert(list.length == old_list.length + 1);

        /* current_size must be at least length * item_size */
        assert(list.current_size >= list.length * list.item_size);

        /* The first element of the list should now match val */
        assert(memcmp(list.data, val, list.item_size) == 0);

        /* val buffer should not have been modified */
        assert_byte_from_buffer_matches(val, &old_val_byte);
    } else {
        /* On failure: length and current_size must not change */
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
    }
}
