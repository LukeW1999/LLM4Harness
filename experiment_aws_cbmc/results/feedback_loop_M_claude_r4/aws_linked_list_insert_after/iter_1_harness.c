#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness() {
    /* Set up a linked list with at least one node so we have a valid 'after' node */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* The 'after' node must be a valid node in the list with a valid next pointer.
     * We use the list head as the 'after' node (always valid, always has a next). */
    struct aws_linked_list_node *after = &list.head;

    /* The node to insert */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* Save state before the call */
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;

    /* Preconditions:
     * - after is non-null and has a valid next pointer
     * - to_add is non-null
     * - after->next is non-null (required by implementation)
     */
    __CPROVER_assume(after != NULL);
    __CPROVER_assume(after->next != NULL);
    __CPROVER_assume(to_add != NULL);

    /* Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* Postconditions from the implementation:
     * to_add->prev = after
     * to_add->next = old after->next
     * old after->next->prev = to_add
     * after->next = to_add
     */

    /* 1. Changed fields: linkage around to_add */
    assert(to_add->prev == after);
    assert(to_add->next == old_after_next);
    assert(after->next == to_add);
    assert(old_after_next->prev == to_add);

    /* 2. Unchanged fields: after->prev should not change */
    assert(after->prev == old_after_prev);

    /* 3. Validity invariant: the list should still be valid */
    assert(aws_linked_list_is_valid(&list));

    /* 4. Node linkage is bidirectional */
    assert(aws_linked_list_node_next_is_valid(after));
    assert(aws_linked_list_node_prev_is_valid(to_add));
}
