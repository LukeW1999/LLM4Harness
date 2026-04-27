/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_init_harness() {
    /* data structure */
    struct aws_linked_list list;

    /* aws_linked_list_init only requires list != NULL, no prior valid state needed */
    __CPROVER_assume(&list != NULL);

    /* perform operation under verification */
    aws_linked_list_init(&list);

    /* assertions */
    /* Postcondition: list is valid */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition: list is empty */
    assert(aws_linked_list_empty(&list));

    /* Postcondition: head.next points to tail */
    assert(list.head.next == &list.tail);

    /* Postcondition: head.prev is NULL */
    assert(list.head.prev == NULL);

    /* Postcondition: tail.prev points to head */
    assert(list.tail.prev == &list.head);

    /* Postcondition: tail.next is NULL */
    assert(list.tail.next == NULL);
}
