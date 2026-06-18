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
     * - to_add->prev == after
     * - to_add->next == old_after_next
     * - old_after_next->prev == to_add
     * - after->next == to_add
     */
    assert(to_add->prev == after);
    assert(to_add->next == old_after_next);
    assert(old_after_next->prev == to_add);
    assert(after->next == to_add);

    /* after->prev should be unchanged */
    assert(after->prev == old_after_prev);

    /* The list should still be valid after insertion */
    assert(aws_linked_list_is_valid(&list));

    /* The list should not be empty after insertion (we inserted at least one node) */
    assert(!aws_linked_list_empty(&list));
}
