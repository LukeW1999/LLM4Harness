/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_linked_list_pop_back
 *
 * From Doxygen: "Returns the element in the back of the list and removes it"
 *
 * Preconditions:
 *   - list is valid
 *   - list is non-empty
 *
 * Postconditions:
 *   - returned node's next and prev are NULL (node was reset)
 *   - list remains valid
 *   - list may now be empty (if it had exactly one element)
 */
void aws_linked_list_pop_back_harness() {
    /* 1. Set up the linked list with non-deterministic contents */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save old state before calling */
    struct aws_linked_list_node *old_back = list.tail.prev;
    struct aws_linked_list_node *old_back_prev = list.tail.prev->prev;

    /* 2. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    /* 3. Assert postconditions */

    /* The returned node must be the old back node */
    assert(result == old_back);

    /* The returned node's next and prev must be NULL (reset) */
    assert(result->next == NULL);
    assert(result->prev == NULL);

    /* The list must remain valid */
    assert(aws_linked_list_is_valid(&list));

    /* The new tail.prev must be the node that was before the removed node */
    assert(list.tail.prev == old_back_prev);

    /* The new last node's next must point to tail */
    assert(list.tail.prev->next == &list.tail);

    /* head.prev must still be NULL (invariant) */
    assert(list.head.prev == NULL);

    /* tail.next must still be NULL (invariant) */
    assert(list.tail.next == NULL);
}
