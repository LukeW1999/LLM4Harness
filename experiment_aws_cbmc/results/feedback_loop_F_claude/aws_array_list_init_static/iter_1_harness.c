/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_static_harness() {
    /* Declare inputs */
    struct aws_array_list list;

    /* Non-deterministic item_count and item_size, bounded to avoid state space explosion */
    size_t item_count;
    size_t item_size;
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Ensure no overflow in item_count * item_size */
    size_t current_size;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size));

    /* Allocate a raw array of the appropriate size */
    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    /* Call function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* Postconditions from Doxygen and implementation */

    /* 1. Changed fields */
    /* alloc is set to NULL (static mode) */
    assert(list.alloc == NULL);

    /* current_size is set to item_count * item_size */
    assert(list.current_size == current_size);

    /* item_size is set to item_size */
    assert(list.item_size == item_size);

    /* length is initialized to 0 */
    assert(list.length == 0);

    /* data points to the provided raw_array */
    assert(list.data == raw_array);

    /* 2. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
