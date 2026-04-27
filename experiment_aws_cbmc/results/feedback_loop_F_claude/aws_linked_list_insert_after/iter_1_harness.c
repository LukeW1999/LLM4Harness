/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_linked_list_insert_after
 *
 * The function inserts to_add immediately after after:
 *   - to_add->prev = after
 *   - to_add->next = after->next (old)
 *   - after->next->prev = to_add  (old after->next now points back to to_add)
 *   - after->next = to_add
 */
void aws_linked_list_insert_after_harness() {
    /* Set up a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* The 'after' node must be a valid node in the list (not the tail,
     * since after->next must be valid). We use the head node as 'after'
     * to ensure after->next is always valid (it's either a real node or tail).
     * Actually, we need a node that has a valid next pointer.
     * We'll use head since head->next is always valid in a valid list. */
    struct aws_linked_list_node *after = &list.head;

    /* Save old state */
    struct aws_linked_list_node *old_after_next = after->next;

    /* The node to insert */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* Precondition: after and to_add are non-null (implied by valid list and malloc) */
    /* Precondition: after->next is valid (guaranteed by valid list) */

    /* Call function under test */
    aws_linked_list_insert_after(after, to_add);

    /* Postconditions: verify the linkage is correct */

    /* 1. to_add->prev points to after */
    assert(to_add->prev == after);

    /* 2. to_add->next points to what was after->next before the call */
    assert(to_add->next == old_after_next);

    /* 3. after->next now points to to_add */
    assert(after->next == to_add);

    /* 4. The old after->next's prev now points to to_add */
    assert(old_after_next->prev == to_add);

    /* 5. The list remains valid after insertion */
    assert(aws_linked_list_is_valid(&list));

    /* 6. The list is no longer empty (we just inserted a node) */
    assert(!aws_linked_list_empty(&list));

    /* 7. Bidirectional linkage: after <-> to_add <-> old_after_next */
    assert(after->next->prev == after);
    assert(to_add->next->prev == to_add);
}
