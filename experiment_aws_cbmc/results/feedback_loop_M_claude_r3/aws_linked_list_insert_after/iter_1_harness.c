#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness() {
    /* Set up a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* We need a node that is already in the list to insert after.
     * We'll use the head sentinel as the "after" node, which is always valid
     * (even in an empty list, head.next == &tail). */
    struct aws_linked_list_node *after = &list.head;

    /* Create a new node to insert */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* Save state before the call */
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;

    /* Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* Postconditions from the implementation:
     * to_add->prev = after
     * to_add->next = old_after_next
     * old_after_next->prev = to_add
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

    /* 4. The list should not be empty after insertion */
    assert(!aws_linked_list_empty(&list));

    /* 5. Bidirectional linkage checks */
    assert(aws_linked_list_node_next_is_valid(after));
    assert(aws_linked_list_node_prev_is_valid(to_add));
}
