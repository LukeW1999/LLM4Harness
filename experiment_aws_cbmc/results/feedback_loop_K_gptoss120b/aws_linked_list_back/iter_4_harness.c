/* Preconditions:
 * - `list` is a non‑null pointer to a valid `aws_linked_list`.
 * - The list is non‑empty (aws_linked_list_back requires this).
 *
 * Additional assumptions:
 * - The back node's `next` field points to the tail sentinel.
 * - The back node's `prev` field points to a valid node (or the head sentinel)
 *   whose `next` field points back to the back node.
 *
 * Postconditions (validity):
 * - The function returns a non‑NULL pointer.
 * - The returned pointer is the node that precedes the tail sentinel
 *   (`list->tail.prev`) and that node's `next` field points to the tail sentinel.
 *
 * Postconditions (frame):
 * - The list's sentinel nodes (`head` and `tail`) are not modified.
 * - No memory outside the list is modified.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_back_harness(void) {
    struct aws_linked_list list;
    make_aws_linked_list(&list);               /* nondet construction */

    /* Assume the list satisfies the function's preconditions. */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Structural assumptions required for the post‑condition. */
    __CPROVER_assume(list.tail.prev != NULL);
    __CPROVER_assume(list.tail.prev->next == &list.tail);
    __CPROVER_assume(list.tail.prev->prev != NULL);
    __CPROVER_assume(list.tail.prev->prev->next == list.tail.prev);

    /* Remember the original state of the sentinel nodes for frame checking. */
    struct aws_linked_list old = list;

    /* Call the function under verification. */
    struct aws_linked_list_node *rval = aws_linked_list_back(&list);

    /* ---- Postconditions (validity) ---- */
    assert(rval != NULL);
    assert(rval == list.tail.prev);
    assert(rval->next == &list.tail);

    /* ---- Postconditions (frame) ---- */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
}
