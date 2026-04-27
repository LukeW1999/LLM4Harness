/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_dynamic_harness() {
    /* Declare inputs */
    struct aws_array_list list;
    struct aws_allocator *alloc = can_fail_allocator();

    /* Bound the inputs to keep state space manageable */
    size_t initial_item_allocation;
    size_t item_size;

    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(alloc->mem_acquire != NULL);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Call function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: item_size and alloc are set */
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        /* length starts at 0 */
        assert(list.length == 0);
        /* If allocation_size > 0, data must be non-null and current_size must equal allocation_size */
        if (initial_item_allocation > 0) {
            assert(list.data != NULL);
            assert(list.current_size == initial_item_allocation * item_size);
        } else {
            /* No allocation needed */
            assert(list.current_size == 0);
        }
        /* The postcondition from the implementation */
        assert(list.current_size == 0 || list.data != NULL);
        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure: function returns AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }
}
