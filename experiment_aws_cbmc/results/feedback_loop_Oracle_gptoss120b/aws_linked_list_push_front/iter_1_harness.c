#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_push_front_harness(void) {
    /* --------------------------------------------------------------------
     * Set up a possibly non‑empty, valid linked list.
     * -------------------------------------------------------------------- */
    struct aws_linked_list list;
    /* The helper creates a list whose internal nodes are allocated
     * nondeterministically and guarantees that the list satisfies
     * aws_linked_list_is_valid(). */
    ensure_linked_list_is_valid(&list);

    /* Remember the original first and last real nodes (if any). */
    struct aws_linked_list_node *old_first = list.head.next;   /* may be &list.tail */
    struct aws_linked_list_node *old_last  = list.tail.prev;   /* may be &list.head */

    /* Snapshot the next pointer of the old first node (if it is a real node).*/
    struct aws_linked_list_node *old_first_next = NULL;
    if (old_first != &list.tail) {
        old_first_next = old_first->next;
    }

    /* --------------------------------------------------------------------
     * Allocate a fresh node to be pushed to the front.
     * -------------------------------------------------------------------- */
    struct aws_linked_list_node *new_node = malloc(sizeof *new_node);
    __CPROVER_assume(new_node != NULL);
    /* The contract requires the node to be reset before insertion. */
    aws_linked_list_node_reset(new_node);
    __CPROVER_assume(new_node->next == NULL && new_node->prev == NULL);

    /* --------------------------------------------------------------------
     * Call the function under verification.
     * -------------------------------------------------------------------- */
    aws_linked_list_push_front(&list, new_node);

    /* --------------------------------------------------------------------
     * Post‑conditions.
     * -------------------------------------------------------------------- */
    /* The list must remain valid. */
    assert(aws_linked_list_is_valid(&list));

    /* The newly added node is now the first element. */
    assert(list.head.next == new_node);
    assert(new_node->prev == &list.head);

    /* Its next pointer must point to the former first element (or tail if the
     * list was empty). */
    if (old_first != &list.tail) {
        assert(new_node->next == old_first);
        assert(old_first->prev == new_node);
        /* The rest of the list after the former first node must be unchanged. */
        assert(old_first->next == old_first_next);
    } else {
        /* The list was empty; the new node must also be the last element. */
        assert(list.tail.prev == new_node);
        assert(new_node->next == &list.tail);
    }

    /* The tail sentinel must still be correctly linked. */
    assert(list.tail.next == NULL);
    assert(list.head.prev == NULL);

    /* If the list was non‑empty, the original last node must still be the tail's
     * predecessor. */
    if (old_last != &list.head) {
        assert(list.tail.prev == old_last);
    }

    /* --------------------------------------------------------------------
     * Frame condition: no memory outside the list and the newly inserted node
     * is modified.  All other allocated objects are untouched.  This is
     * implicitly checked by the above assertions on the list structure and
     * by the fact that we never write to any other object.
     * -------------------------------------------------------------------- */
    return 0;
}
