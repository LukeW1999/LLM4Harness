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
       which is always valid (inserting before tail = push_back semantics),
       or we use head.next if the list is non-empty. */

    /* Create to_add node */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* Choose "before" node: use list.tail (the sentinel tail) as the insertion point.
       This is always valid since tail always has a valid prev pointer. */
    struct aws_linked_list_node *before = &list.tail;

    /* Save old state */
    struct aws_linked_list_node *old_before_prev = before->prev;

    /* Preconditions:
       - before must be a valid node with a valid prev pointer
       - before->prev must not be NULL (always true for a valid list)
    */
    __CPROVER_assume(before->prev != NULL);
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(before));

    /* 3. Call function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 4. Assert postconditions */

    /* to_add->next should point to before */
    assert(to_add->next == before);

    /* to_add->prev should point to old before->prev */
    assert(to_add->prev == old_before_prev);

    /* before->prev should now point to to_add */
    assert(before->prev == to_add);

    /* old_before_prev->next should now point to to_add */
    assert(old_before_prev->next == to_add);

    /* The bidirectional links are correct */
    assert(aws_linked_list_node_next_is_valid(to_add));
    assert(aws_linked_list_node_prev_is_valid(to_add));

    /* The list should still be valid */
    assert(aws_linked_list_is_valid(&list));
}
