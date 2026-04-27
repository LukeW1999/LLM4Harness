/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_back_harness() {
    /* data structure */
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Precondition: list must be valid and non-empty */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Keep the old back node and the node before it */
    struct aws_linked_list_node *old_back = list.tail.prev;
    struct aws_linked_list_node *old_second_to_last = old_back->prev;

    /* perform operation under verification */
    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    /* assertions */

    /* 1. Changed fields: the returned node is the old back node */
    assert(result == old_back);

    /* 2. The returned node's next and prev are NULL after removal */
    assert(result->next == NULL);
    assert(result->prev == NULL);

    /* 3. The list is still valid after the operation */
    assert(aws_linked_list_is_valid(&list));

    /* 4. The new back of the list is the node that was second-to-last */
    assert(list.tail.prev == old_second_to_last);

    /* 5. The new back node's next points to tail */
    assert(old_second_to_last->next == &list.tail);

    /* 6. Tail's next is still NULL (unchanged) */
    assert(list.tail.next == NULL);

    /* 7. Head's prev is still NULL (unchanged) */
    assert(list.head.prev == NULL);
}
