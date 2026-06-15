/*  CBMC harness for aws_linked_list_insert_before()
 *
 *  The function inserts a node (to_add) immediately before another node
 *  (before) in a doubly‑linked list.  It updates the four pointers that
 *  connect the new node with its neighbours.
 *
 *  The harness:
 *   - allocates a list and ensures it is valid,
 *   - creates at least one real node so that we have a non‑sentinel
 *     “before” candidate,
 *   - nondeterministically chooses a “before” node that is either the
 *     first real element or the tail sentinel,
 *   - allocates a fresh node to add (its next/prev are NULL),
 *   - saves the relevant old state,
 *   - calls the function,
 *   - asserts the expected pointer relationships,
 *   - asserts that the list remains valid,
 *   - asserts that fields that must not change indeed stay unchanged.
 */

#include <assert.h>
#include <stdlib.h>
#include "aws/common/linked_list.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_insert_before_harness(void) {
    /* 1. Allocate and bound a list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Build a list with at least one element so that we have a
     *    non‑sentinel node to use as a possible “before”. */
    struct aws_linked_list_node *first = malloc(sizeof(*first));
    __CPROVER_assume(first != NULL);
    aws_linked_list_node_reset(first);
    aws_linked_list_push_back(&list, first);

    /* 3. Choose a node to insert before.
     *    - Either the tail sentinel (push_back case) or
     *    - The first real element (insert before head of data). */
    struct aws_linked_list_node *before;
    if (nondet_bool()) {
        before = &list.tail;               /* insert before tail sentinel */
    } else {
        before = list.head.next;            /* insert before first element */
    }

    /* 4. Allocate the node that will be inserted.  It must not already be
     *    part of a list, so we reset its links. */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);      /* next == prev == NULL */

    /* 5. Save old state needed for post‑condition checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_before_prev = before->prev;

    /* 6. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 7. Post‑conditions --------------------------------------------------- */

    /* The newly added node is linked correctly */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(aws_linked_list_node_is_in_list(to_add));

    /* The “before” node now points back to the new node */
    assert(before->prev == to_add);

    /* The node that previously preceded “before” now points forward to the new node.
     * This is not true when the previous node is the head sentinel. */
    if (old_before_prev != &list.head) {
        assert(old_before_prev->next == to_add);
    }

    /* List validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* Fields of the list structure that must never change */
    assert(list.head.prev == old_list.head.prev);   /* should still be &list.head */
    assert(list.tail.next == old_list.tail.next);   /* should still be &list.tail */
}
