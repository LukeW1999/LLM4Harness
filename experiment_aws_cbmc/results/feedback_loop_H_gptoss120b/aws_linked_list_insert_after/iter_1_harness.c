/*  Harness for aws_linked_list_insert_after()
 *
 *  This harness verifies that inserting a node after a given node updates
 *  the list connectivity exactly as specified and that the list remains
 *  valid afterwards.
 */

#include <aws/common/linked_list.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* The function requires a non‑empty list so that there is a node
     * after which we can insert. */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 2. Choose a non‑sentinel node from the list to act as `after`.
     *    We walk a nondeterministic number of steps (bounded) starting
     *    from the first real node. */
    struct aws_linked_list_node *after = list.head.next;
    size_t steps = nondet_size_t();
    __CPROVER_assume(steps < MAX_LINKED_LIST_ITEM_ALLOCATION);
    while (steps--) {
        after = after->next;
        /* Stop before reaching the tail sentinel. */
        __CPROVER_assume(after != &list.tail);
    }

    /* 3. Allocate a fresh node to be inserted. */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    /* The node must not already be part of a list. */
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(to_add->next == NULL && to_add->prev == NULL);

    /* 4. Save old state for later comparison. */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_after = *after;
    struct aws_linked_list_node *old_next = after->next;          /* original next node */
    struct aws_linked_list_node old_next_node = *old_next;       /* copy of that node */

    /* 5. Call the function under test. */
    aws_linked_list_insert_after(after, to_add);

    /* --------------------------------------------------------------------
     *  Post‑condition checks
     * -------------------------------------------------------------------- */

    /* The four pointers that the implementation updates must have the
     * expected values. */
    assert(after->next == to_add);                /* after now points to the new node */
    assert(to_add->prev == after);                /* new node's prev points back to after */
    assert(to_add->next == old_next);             /* new node's next is the original successor */
    assert(old_next->prev == to_add);              /* original successor's prev now points to new node */

    /* The list head sentinel must remain unchanged. */
    assert(list.head.prev == old_list.head.prev);
    assert(list.head.next == old_list.head.next);

    /* The list tail sentinel itself does not change, but its `prev`
     * pointer may be updated if we inserted after the former last node. */
    if (old_next == &list.tail) {
        /* Inserting after the former last element makes the new node the
         * last element, therefore tail.prev must now be the new node. */
        assert(list.tail.prev == to_add);
    } else {
        /* Otherwise tail.prev is unchanged. */
        assert(list.tail.prev == old_list.tail.prev);
    }

    /* No other node in the list should have been altered.  We walk the
     * list and verify that every node other than `after`, `to_add`,
     * and the original successor (`old_next`) retains its previous
     * `prev` and `next` pointers. */
    for (struct aws_linked_list_node *cur = list.head.next;
         cur != &list.tail;
         cur = cur->next) {
        if (cur != after && cur != to_add && cur != old_next) {
            /* Find the corresponding node in the old snapshot.  Because the
             * list structure (except for the insertion point) is unchanged,
             * the address of each such node is the same in the old copy. */
            struct aws_linked_list_node *old_cur = cur;
            assert(old_cur->prev == ((struct aws_linked_list_node *)old_cur)->prev);
            assert(old_cur->next == ((struct aws_linked_list_node *)old_cur)->next);
        }
    }

    /* --------------------------------------------------------------------
     *  Invariant checks
     * -------------------------------------------------------------------- */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
