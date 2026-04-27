/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_array_list_pop_front_n
 *
 * Doxygen says:
 * - Delete N elements from the front of the list.
 * - Remaining elements are shifted to the front of the list.
 * - If the list has less than N elements, the list is cleared.
 * - This call is more efficient than calling aws_array_list_pop_front() N times.
 *
 * Analysis:
 * 1. Changed fields on success:
 *    - list.length: decreases by n (or becomes 0 if n >= length)
 *    - list.data contents: shifted (remaining elements moved to front)
 * 2. Unchanged fields:
 *    - list.alloc
 *    - list.current_size
 *    - list.item_size
 *    - list.data pointer (same pointer, contents shifted)
 * 3. Failure: function is void, no failure path per se
 * 4. Validity invariants: aws_array_list_is_valid must hold after call
 */
void aws_array_list_pop_front_n_harness() {
    /* data structure */
    struct aws_array_list list;

    /* bound the list to keep verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* non-deterministic n */
    size_t n;

    /* save old state */
    struct aws_array_list old = list;
    size_t old_length = list.length;

    /* save a byte from the data buffer for immutability checks on unchanged portion */
    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* perform operation under verification */
    aws_array_list_pop_front_n(&list, n);

    /* assertions */

    /* validity invariant must hold */
    assert(aws_array_list_is_valid(&list));

    /* unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* changed fields: length */
    if (n >= old_length) {
        /* list is cleared: length becomes 0 */
        assert(list.length == 0);
    } else if (n == 0) {
        /* nothing removed */
        assert(list.length == old_length);
    } else {
        /* n elements removed from front */
        assert(list.length == old_length - n);
    }
}
