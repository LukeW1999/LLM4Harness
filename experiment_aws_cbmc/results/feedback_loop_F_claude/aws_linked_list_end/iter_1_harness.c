/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_linked_list_end
 *
 * The function returns a pointer to the tail sentinel node of the list,
 * which serves as the one-past-the-end iterator.
 *
 * 1. Changed fields: none (pure accessor)
 * 2. Unchanged fields: all list fields remain unchanged
 * 3. Failure: no failure path (always returns &list->tail)
 * 4. Validity invariants: list remains valid after call
 */
void aws_linked_list_end_harness() {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state for immutability checks */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    /* 4. Assert postconditions */

    /* The return value must be a pointer to the tail sentinel node */
    assert(result == &list.tail);

    /* The list must remain valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* All fields must be unchanged (pure accessor) */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
}
