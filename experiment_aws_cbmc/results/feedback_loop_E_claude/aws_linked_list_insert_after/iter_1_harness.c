/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_after_harness() {
    /* data structure */
    struct aws_linked_list list;
    struct aws_linked_list_node to_add;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume the list is valid before the operation */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Non-deterministically pick a node in the list to insert after.
     * We use the head node as a valid "after" node (it always exists). */
    struct aws_linked_list_node *after = list.head.next;
    /* after must be a valid node (not the tail, so next is valid) */
    __CPROVER_assume(after != NULL);
    __CPROVER_assume(after->next != NULL);
    __CPROVER_assume(aws_linked_list_node_next_is_valid(after));

    /* Save the old next of after */
    struct aws_linked_list_node *old_next = after->next;

    /* perform operation under verification */
    aws_linked_list_insert_after(after, &to_add);

    /* assertions */
    /* to_add's prev should point to after */
    assert(to_add.prev == after);
    /* to_add's next should point to old_next */
    assert(to_add.next == old_next);
    /* after's next should now point to to_add */
    assert(after->next == &to_add);
    /* old_next's prev should now point to to_add */
    assert(old_next->prev == &to_add);

    /* Bidirectional linkage checks */
    assert(aws_linked_list_node_next_is_valid(after));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));

    /* List validity */
    assert(aws_linked_list_is_valid(&list));
}
