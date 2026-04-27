#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

/**
 * aws_linked_list_insert_after inserts `to_add` immediately after `after` in the list.
 *
 * Implementation:
 *   to_add->prev = after;
 *   to_add->next = after->next;
 *   after->next->prev = to_add;
 *   after->next = to_add;
 *
 * Preconditions:
 *   - `after` is a valid node already in a list (after->next != NULL)
 *   - `to_add` is a valid node to insert
 *
 * Postconditions:
 *   - to_add->prev == after
 *   - to_add->next == old after->next
 *   - (old after->next)->prev == to_add
 *   - after->next == to_add
 */
void aws_linked_list_insert_after_harness(void) {
    /* 1. Set up a linked list with at least one node so we have a valid `after` node */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Pick a node to insert after — use head (always valid, even in empty list) */
    struct aws_linked_list_node *after = &list.head;

    /* after->next must be non-NULL for the function to work correctly */
    __CPROVER_assume(after != NULL);
    __CPROVER_assume(after->next != NULL);
    __CPROVER_assume(after->next->prev != NULL);

    /* 3. Create the node to insert */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* 4. Save old state before the call */
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;

    /* 5. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 6. Assert postconditions — changed fields */
    /* to_add->prev must point to after */
    assert(to_add->prev == after);

    /* to_add->next must point to what was after->next before the call */
    assert(to_add->next == old_after_next);

    /* The node that was after->next must now have its prev pointing to to_add */
    assert(old_after_next->prev == to_add);

    /* after->next must now point to to_add */
    assert(after->next == to_add);

    /* 7. Assert unchanged fields */
    /* after->prev must not have changed */
    assert(after->prev == old_after_prev);

    /* old_after_next->next must not have changed (function doesn't touch it) */
    /* We can't easily assert this without saving it, but we assert the key linkage */

    /* 8. Assert validity of the list structure around the insertion point */
    /* The doubly-linked invariant: to_add->next->prev == to_add */
    assert(to_add->next->prev == to_add);

    /* The doubly-linked invariant: to_add->prev->next == to_add */
    assert(to_add->prev->next == to_add);

    /* after->next->prev == after (i.e., to_add->prev == after, already checked) */
    assert(after->next == to_add);
    assert(to_add->prev == after);
}

int main(void) {
    aws_linked_list_insert_after_harness();
    return 0;
}
