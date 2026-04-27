/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <string.h>

void aws_array_list_swap_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.length > 0);
    __CPROVER_assume(list.item_size > 0);

    size_t index_a;
    size_t index_b;
    __CPROVER_assume(index_a < list.length);
    __CPROVER_assume(index_b < list.length);

    /* Save old list metadata */
    void *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;

    size_t item_size = list.item_size;

    /* Check validity */
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(index_a * item_size + item_size <= list.current_size);
    __CPROVER_assume(index_b * item_size + item_size <= list.current_size);

    /* Call the function under test */
    aws_array_list_swap(&list, index_a, index_b);

    /* Check validity */
    assert(aws_array_list_is_valid(&list));

    /* Check metadata unchanged */
    assert(list.alloc == old_alloc);
    assert(list.current_size == old_current_size);
    assert(list.length == old_length);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);
}
