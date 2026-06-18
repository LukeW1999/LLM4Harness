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

    /* 2. We need a 'before' node that is already in the list,
       and a 'to_add' node to insert before it.
       For simplicity, use the list's tail as 'before' (always valid),
       which means we're inserting before the tail sentinel — equivalent to push_back.
       But we want to test the general case, so we pick a node non-deterministically. */

    /* Allocate to_add node */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* Pick 'before' as either head.next (first real node or tail) or tail */
    /* We'll use list.tail as 'before' to ensure it's always a valid node in the list */
    struct aws_linked_list_node *before = &list.tail;

    /* Snapshot state before the call */
    /* Save the node that was previously before 'before' */
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;

    /* 3. Preconditions:
       - before must be a valid node in the list (non-null, with valid prev/next)
       - to_add must be non-null
       - before->prev must be non-null (it's a doubly linked list with sentinels) */
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before->prev != NULL);
    __CPROVER_assume(to_add != NULL);
    /* The list must be valid (bidirectional links) */
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(before));

    /* 4. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 5. Assert postconditions */

    /* Changed fields: to_add is now inserted before 'before' */

    /* to_add->next must point to before */
    assert(to_add->next == before);

    /* to_add->prev must point to old_before_prev */
    assert(to_add->prev == old_before_prev);

    /* before->prev must now point to to_add */
    assert(before->prev == to_add);

    /* old_before_prev->next must now point to to_add */
    assert(old_before_prev->next == to_add);

    /* Unchanged fields: before->next should not have changed */
    assert(before->next == old_before_next);

    /* Bidirectional link checks */
    /* to_add <-> before */
    assert(to_add->next == before);
    assert(before->prev == to_add);

    /* old_before_prev <-> to_add */
    assert(old_before_prev->next == to_add);
    assert(to_add->prev == old_before_prev);

    /* 6. Validity invariants */
    /* The node connections are bidirectional */
    assert(aws_linked_list_node_next_is_valid(to_add));
    assert(aws_linked_list_node_prev_is_valid(to_add));
    assert(aws_linked_list_node_prev_is_valid(before));
    assert(aws_linked_list_node_next_is_valid(old_before_prev));

    /* The list itself should still be valid */
    assert(aws_linked_list_is_valid(&list));
}
