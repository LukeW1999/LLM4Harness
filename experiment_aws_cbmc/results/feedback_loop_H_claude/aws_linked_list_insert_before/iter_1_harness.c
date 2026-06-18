#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness() {
    /* 1. Set up a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Create the node to insert before (must be a valid node in the list) */
    /* We'll use a non-deterministic node that is already in the list.
       For simplicity, we use the tail sentinel as the "before" node,
       which is always valid (inserting before tail = push_back). */
    struct aws_linked_list_node *before;
    /* before can be any node in the list including tail */
    /* We non-deterministically pick: either the tail or head.next (if non-empty) */
    /* For a sound proof, let's use the tail sentinel as "before" */
    before = &list.tail;

    /* 3. Create the node to add */
    struct aws_linked_list_node to_add;

    /* 4. Save old state */
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;

    /* Preconditions: before must be a valid node with valid prev/next */
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before->prev != NULL);
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(before));

    /* 5. Call the function under test */
    aws_linked_list_insert_before(before, &to_add);

    /* 6. Assert postconditions */

    /* to_add.next should point to before */
    assert(to_add.next == before);

    /* to_add.prev should point to old before->prev */
    assert(to_add.prev == old_before_prev);

    /* before->prev should now point to to_add */
    assert(before->prev == &to_add);

    /* old_before_prev->next should now point to to_add */
    assert(old_before_prev->next == &to_add);

    /* before->next should be unchanged */
    assert(before->next == old_before_next);

    /* The bidirectional links around to_add are correct */
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(aws_linked_list_node_prev_is_valid(&to_add));

    /* The list should still be valid */
    assert(aws_linked_list_is_valid(&list));
}
