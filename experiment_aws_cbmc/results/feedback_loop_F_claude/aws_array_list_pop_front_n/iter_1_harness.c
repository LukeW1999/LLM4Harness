/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic n */
    size_t n = nondet_size_t();

    /* 3. Save old state */
    struct aws_array_list old = list;
    size_t old_length = list.length;

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Assert postconditions */

    /* Fields that must NOT change */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Behavioral postconditions based on Doxygen:
     * - If list has less than N elements, the list is cleared (length == 0)
     * - Otherwise, N elements are removed from the front, remaining elements shifted
     */
    if (n >= old_length) {
        /* List should be cleared */
        assert(list.length == 0);
    } else if (n == 0) {
        /* Nothing changes */
        assert(list.length == old_length);
    } else {
        /* n elements removed from front */
        assert(list.length == old_length - n);
    }

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
