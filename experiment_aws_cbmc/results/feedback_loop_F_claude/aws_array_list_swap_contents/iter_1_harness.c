/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_swap_contents_harness() {
    /* Declare two array lists */
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    /* Bound the lists to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list_a, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_bounded(&list_b, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Allocate data members */
    ensure_array_list_has_allocated_data_member(&list_a);
    ensure_array_list_has_allocated_data_member(&list_b);

    /* Assume both lists are valid */
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* Preconditions from the implementation:
     * - list_a->alloc must be non-null
     * - list_a->alloc == list_b->alloc (same allocator)
     * - list_a->item_size == list_b->item_size (same item size)
     * - list_a != list_b (different pointers)
     */
    __CPROVER_assume(list_a.alloc != NULL);
    __CPROVER_assume(list_a.alloc == list_b.alloc);
    __CPROVER_assume(list_a.item_size == list_b.item_size);
    /* list_a and list_b are different local variables, so &list_a != &list_b is always true */

    /* Save old state before the call */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    /* Call the function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* Postconditions: contents are swapped */
    /* list_a should now have list_b's old contents */
    assert(list_a.alloc == old_b.alloc);
    assert(list_a.current_size == old_b.current_size);
    assert(list_a.length == old_b.length);
    assert(list_a.item_size == old_b.item_size);
    assert(list_a.data == old_b.data);

    /* list_b should now have list_a's old contents */
    assert(list_b.alloc == old_a.alloc);
    assert(list_b.current_size == old_a.current_size);
    assert(list_b.length == old_a.length);
    assert(list_b.item_size == old_a.item_size);
    assert(list_b.data == old_a.data);

    /* Both lists must remain valid after the swap */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
