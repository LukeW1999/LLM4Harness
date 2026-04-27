/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_array_list_erase
 *
 * Analysis:
 * 1. Changed fields on success: list->length decreases by 1 (element removed)
 * 2. Unchanged fields: alloc, item_size, current_size, data pointer
 * 3. On failure (index >= length): returns AWS_OP_ERR, list unchanged
 * 4. Validity invariants: aws_array_list_is_valid must hold after call
 */
void aws_array_list_erase_harness() {
    /* data structure */
    struct aws_array_list list;

    /* bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* non-deterministic index */
    size_t index;

    /* save old state */
    struct aws_array_list old = list;

    /* save a byte from the data buffer for immutability checks on failure */
    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* perform operation under verification */
    int result = aws_array_list_erase(&list, index);

    /* assertions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: index must have been valid (index < old.length) */
        assert(index < old.length);
        /* length decreases by 1 */
        assert(list.length == old.length - 1);
        /* item_size unchanged */
        assert(list.item_size == old.item_size);
        /* allocator unchanged */
        assert(list.alloc == old.alloc);
        /* current_size unchanged */
        assert(list.current_size == old.current_size);
        /* data pointer unchanged */
        assert(list.data == old.data);
    } else {
        /* On failure: index >= length, list is unchanged */
        assert(index >= old.length);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        /* data contents unchanged */
        if (old.current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }

    /* validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
