/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_ptr_harness() {
    /* data structure */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state before calling */
    struct aws_array_list old = list;

    /* Non-deterministic index */
    size_t index;

    /* Output pointer */
    void *val;

    /* Call function under test */
    int result = aws_array_list_get_at_ptr(&list, &val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: index < length, val points into the list data */
        assert(list.length > index);
        assert(val == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        /* Failure: index >= length */
        assert(list.length <= index);
    }

    /* Unchanged fields regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
