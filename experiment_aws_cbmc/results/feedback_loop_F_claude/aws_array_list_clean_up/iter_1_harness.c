/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clean_up_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Call the function under test */
    aws_array_list_clean_up(&list);

    /* 3. Assert postconditions:
     *    aws_array_list_clean_up deallocates memory and zeros the struct.
     *    After AWS_ZERO_STRUCT(*list), all fields should be zero/NULL.
     */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);

    /* 4. Assert validity invariant:
     *    A zeroed-out list should still be considered valid by aws_array_list_is_valid,
     *    since data == NULL and current_size == 0 is a valid state (static or uninitialized).
     */
    assert(aws_array_list_is_valid(&list));
}
