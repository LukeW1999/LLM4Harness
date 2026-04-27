/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_linked_list_init
 *
 * Doxygen: "Initializes the list. List will be empty after this call."
 *
 * Analysis:
 * 1. Changed fields on success:
 *    - list->head.next = &list->tail
 *    - list->head.prev = NULL
 *    - list->tail.prev = &list->head
 *    - list->tail.next = NULL
 * 2. Unchanged fields: none (all fields are set by init)
 * 3. Failure: function has no failure path (void return, precondition: list != NULL)
 * 4. Validity invariants: aws_linked_list_is_valid(list) and aws_linked_list_empty(list)
 */
void aws_linked_list_init_harness() {
    /* Declare the list - just needs to be non-null (stack allocated) */
    struct aws_linked_list list;

    /* Call function under test */
    aws_linked_list_init(&list);

    /* Assert postconditions from Doxygen and implementation */

    /* 1. Changed fields: head.next points to tail */
    assert(list.head.next == &list.tail);

    /* 2. Changed fields: head.prev is NULL */
    assert(list.head.prev == NULL);

    /* 3. Changed fields: tail.prev points to head */
    assert(list.tail.prev == &list.head);

    /* 4. Changed fields: tail.next is NULL */
    assert(list.tail.next == NULL);

    /* 5. Validity invariant: list is valid */
    assert(aws_linked_list_is_valid(&list));

    /* 6. Validity invariant: list is empty after init */
    assert(aws_linked_list_empty(&list));
}
