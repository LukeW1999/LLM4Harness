/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_array_list_swap
 *
 * The function swaps elements at indices a and b in the list.
 * Preconditions: a < list->length, b < list->length
 * Postconditions:
 *   - The data at index a and b are swapped
 *   - All other fields (alloc, current_size, length, item_size, data pointer) remain unchanged
 *   - The list remains valid
 */
void aws_array_list_swap_harness() {
    /* data structure */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    /* Ensure the list has at least one element so indices can be valid */
    __CPROVER_assume(list.length > 0);

    /* Non-deterministic indices */
    size_t a;
    size_t b;
    /* Preconditions from AWS_FATAL_PRECONDITION */
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* Save old state */
    struct aws_array_list old_list = list;

    /* Save bytes at positions a and b before the swap */
    size_t item_size = list.item_size;
    uint8_t *data = (uint8_t *)list.data;

    /* Save a copy of the element at index a */
    uint8_t *elem_a_before = malloc(item_size);
    __CPROVER_assume(elem_a_before != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(data + a * item_size, item_size));
    __CPROVER_assume(AWS_MEM_IS_READABLE(data + b * item_size, item_size));
    memcpy(elem_a_before, data + a * item_size, item_size);

    /* Save a copy of the element at index b */
    uint8_t *elem_b_before = malloc(item_size);
    __CPROVER_assume(elem_b_before != NULL);
    memcpy(elem_b_before, data + b * item_size, item_size);

    /* perform operation under verification */
    aws_array_list_swap(&list, a, b);

    /* assertions */

    /* Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));

    /* Unchanged fields */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /* Check swap behavior */
    if (a == b) {
        /* If a == b, elements should be unchanged */
        assert_bytes_match(data + a * item_size, elem_a_before, item_size);
        assert_bytes_match(data + b * item_size, elem_b_before, item_size);
    } else {
        /* If a != b, elements should be swapped:
         * data[a] should now contain what was at data[b]
         * data[b] should now contain what was at data[a]
         */
        assert_bytes_match(data + a * item_size, elem_b_before, item_size);
        assert_bytes_match(data + b * item_size, elem_a_before, item_size);
    }
}
