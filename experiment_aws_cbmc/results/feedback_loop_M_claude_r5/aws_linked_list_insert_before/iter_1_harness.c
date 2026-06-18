#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness() {
    /* Set up a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Create the node to insert before (must be a valid node in the list) */
    /* We'll use a node that is already in the list - pick a nondet node */
    /* For simplicity, we'll insert before the tail sentinel, which is always valid */
    struct aws_linked_list_node *before = &list.tail;

    /* Create the node to add */
    struct aws_linked_list_node to_add;

    /* Save state before the call */
    struct aws_linked_list_node *old_before_prev = before->prev;

    /* Preconditions:
     * - before must be a valid node (not NULL)
     * - before->prev must be valid (not NULL)
     * - The list must be valid
     */
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before->prev != NULL);
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(before));

    /* Call the function under test */
    aws_linked_list_insert_before(before, &to_add);

    /* Postconditions:
     * 1. to_add->next == before
     * 2. to_add->prev == old_before_prev
     * 3. old_before_prev->next == &to_add
     * 4. before->prev == &to_add
     */
    assert(to_add.next == before);
    assert(to_add.prev == old_before_prev);
    assert(old_before_prev->next == &to_add);
    assert(before->prev == &to_add);

    /* Bidirectional linkage checks */
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(aws_linked_list_node_prev_is_valid(&to_add));

    /* The list should still be valid after insertion */
    assert(aws_linked_list_is_valid(&list));
}
