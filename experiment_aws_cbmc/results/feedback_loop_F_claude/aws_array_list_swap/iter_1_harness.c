/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_swap_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic indices */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 3. Preconditions: both indices must be within bounds */
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* Save the bytes at positions a and b before the swap */
    uint8_t *data = (uint8_t *)list.data;
    size_t item_size = list.item_size;

    /* Allocate buffers to save old contents at indices a and b */
    uint8_t *old_a_data = malloc(item_size);
    uint8_t *old_b_data = malloc(item_size);
    __CPROVER_assume(old_a_data != NULL);
    __CPROVER_assume(old_b_data != NULL);

    if (item_size > 0 && list.data != NULL) {
        memcpy(old_a_data, data + a * item_size, item_size);
        memcpy(old_b_data, data + b * item_size, item_size);
    }

    /* 5. Call function under test */
    aws_array_list_swap(&list, a, b);

    /* 6. Assert postconditions */

    /* Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));

    /* Unchanged fields: alloc, current_size, length, item_size, data pointer */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /* If a == b, nothing changes */
    if (a == b) {
        if (item_size > 0 && list.data != NULL) {
            assert(memcmp(data + a * item_size, old_a_data, item_size) == 0);
        }
    } else {
        /* After swap: element at index a should contain old element at b,
           and element at index b should contain old element at a */
        if (item_size > 0 && list.data != NULL) {
            assert(memcmp(data + a * item_size, old_b_data, item_size) == 0);
            assert(memcmp(data + b * item_size, old_a_data, item_size) == 0);
        }
    }
}
