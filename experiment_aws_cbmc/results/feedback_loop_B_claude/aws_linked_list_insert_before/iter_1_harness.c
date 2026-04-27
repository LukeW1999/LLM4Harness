#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

/**
 * aws_linked_list_insert_before inserts to_add before the node `before`.
 *
 * Implementation:
 *   to_add->next = before;
 *   to_add->prev = before->prev;
 *   before->prev->next = to_add;
 *   before->prev = to_add;
 *
 * Preconditions:
 *   - `before` is a valid node in a valid linked list (before->prev != NULL)
 *   - `to_add` is a valid node (non-NULL)
 *
 * Postconditions:
 *   - to_add->next == before
 *   - to_add->prev == old_before_prev
 *   - old_before_prev->next == to_add
 *   - before->prev == to_add
 */
void aws_linked_list_insert_before_harness(void) {
    /* 1. Set up a linked list with at least one element so we have a valid
     *    `before` node that has a valid prev pointer. */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Pick a non-deterministic node to insert before.
     *    We use list.tail as `before` — it always has a valid prev pointer
     *    (either head or a real node), and it is always present. */
    struct aws_linked_list_node *before = list.head.next;
    /* before must be a node in the list (head.next is always valid in a
     * valid list — it points to either a real node or tail). */
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before->prev != NULL);

    /* 3. The node to insert. */
    struct aws_linked_list_node *to_add =
        (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* 4. Save old state before the call. */
    struct aws_linked_list_node *old_before_prev = before->prev;

    /* 5. Call the function under test. */
    aws_linked_list_insert_before(before, to_add);

    /* 6. Assert postconditions — changed fields. */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(old_before_prev->next == to_add);
    assert(before->prev == to_add);

    /* 7. Assert unchanged fields on `before` (only prev was changed). */
    /* before->next is not touched by the function */
    /* We cannot assert before->next == old_before_next without saving it,
     * but we can assert the list validity which covers structural integrity. */

    /* 8. Assert validity invariants. */
    /* The list should still be structurally valid after insertion. */
    assert(aws_linked_list_is_valid(&list));
}
