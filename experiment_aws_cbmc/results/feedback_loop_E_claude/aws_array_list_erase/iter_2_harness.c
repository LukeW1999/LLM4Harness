/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

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
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data = list.data;

    /* perform operation under verification */
    int result = aws_array_list_erase(&list, index);

    /* validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size and allocator never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);
    assert(list.current_size == old_current_size);
    assert(list.data == old_data);

    if (result == AWS_OP_SUCCESS) {
        /* On success: index must have been valid (index < old_length) */
        assert(index < old_length);
        /* length decreases by 1 */
        assert(list.length == old_length - 1);
    } else {
        /* On failure: list length is unchanged */
        assert(list.length == old_length);
    }
}
