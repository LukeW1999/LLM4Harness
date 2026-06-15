/* Preconditions:
 * - `list` is a non‑null pointer to a valid `aws_linked_list` (i.e., `aws_linked_list_is_valid(list) == true`).
 *
 * Postconditions (validity):
 * - The function returns a non‑NULL pointer.
 * - If the list is empty (`aws_linked_list_empty(list)`), the returned pointer is the list's head sentinel (`&list->head`).
 * - Otherwise the returned pointer is the node that precedes the tail sentinel (`list->tail.prev`) and that node's `next` field points to the tail sentinel (`rval->next == &list->tail`).
 *
 * Postconditions (frame):
 * - The list structure (the `head` and `tail` nodes) is not modified by the call.
 * - No memory outside the list is modified.
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_back_harness(void) {
    /* Set up a possibly empty, but otherwise valid linked list. */
    struct aws_linked_list list;
    make_aws_linked_list(&list);               /* nondet construction, guarantees validity */

    /* Remember the original state of the list's sentinel nodes for frame checking. */
    struct aws_linked_list old = list;

    /* Call the function under verification. */
    struct aws_linked_list_node *rval = aws_linked_list_back(&list);

    /* ---- Postconditions (validity) ---- */
    assert(rval != NULL);

    if (aws_linked_list_empty(&list)) {
        /* Empty list: tail.prev points to head, so back() should return the head sentinel. */
        assert(rval == &list.head);
    } else {
        /* Non‑empty list: back() must return the node just before the tail sentinel. */
        assert(rval == list.tail.prev);
        assert(rval->next == &list.tail);
    }

    /* ---- Postconditions (frame) ---- */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    return 0;
}
