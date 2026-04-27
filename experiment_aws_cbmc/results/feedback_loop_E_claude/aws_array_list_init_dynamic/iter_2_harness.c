/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_array_list_init_dynamic
 */
void aws_array_list_init_dynamic_harness() {
    /* data structure */
    struct aws_array_list list; /* Precondition: list is non-null */

    /* parameters */
    struct aws_allocator *alloc = aws_default_allocator();
    size_t item_size;
    size_t initial_item_allocation;

    /* assumptions */
    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* perform operation under verification */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* assertions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: list is valid */
        assert(aws_array_list_is_valid(&list));

        /* item_size and alloc are set */
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);

        /* length starts at 0 */
        assert(list.length == 0);

        /* If initial_item_allocation > 0, current_size == initial_item_allocation * item_size and data != NULL */
        /* If initial_item_allocation == 0, current_size == 0 and data == NULL */
        if (initial_item_allocation == 0) {
            assert(list.current_size == 0);
            assert(list.data == NULL);
        } else {
            size_t expected_size;
            /* We know multiplication didn't overflow since we succeeded */
            assert(!aws_mul_size_checked(initial_item_allocation, item_size, &expected_size));
            assert(list.current_size == expected_size);
            assert(list.data != NULL);
        }

        /* Postcondition from implementation: current_size == 0 || data != NULL */
        assert(list.current_size == 0 || list.data != NULL);
    } else {
        /* On failure: result is AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
    }
}
