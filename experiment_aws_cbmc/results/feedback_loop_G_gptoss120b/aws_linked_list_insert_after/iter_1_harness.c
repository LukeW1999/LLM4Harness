#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save a copy of the whole list for unchanged‑field checks */
    struct aws_linked_list old_list = list;

    /* 2. Choose a nondeterministic node that is already in the list
       (or the head sentinel, which is also a valid insertion point). */
    struct aws_linked_list_node *after = nondet_bool() ? &list.head : nondet_bool() ? &list.tail : (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(after != NULL);
    /* If we allocated a fresh node, force it to be part of the list by linking it temporarily.
       This keeps the harness simple; the important thing is that `after->next` is a valid node. */
    if (after != &list.head && after != &list.tail) {
        /* link the fresh node into the list as a temporary element */
        after->prev = &list.head;
        after->next = list.head.next;
        list.head.next->prev = after;
        list.head.next = after;
    }

    /* 3. Allocate a node to be inserted and ensure it is not already in a list */
    struct aws_linked_list_node *to_add = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* 4. Save the node that currently follows `after` */
    struct aws_linked_list_node *old_after_next = after->next;

    /* 5. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 6. Post‑condition: linkage updates */
    assert(after->next == to_add);
    assert(to_add->prev == after);
    assert(to_add->next == old_after_next);
    assert(old_after_next->prev == to_add);

    /* 7. The inserted node is now considered part of a list */
    assert(aws_linked_list_node_is_in_list(to_add));

    /* 8. Unchanged fields of the list (those not touched by the insertion) */
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    /* The tail sentinel's `prev` is unchanged unless we inserted after the tail,
       which is undefined behaviour; we therefore assert it unchanged as well. */
    assert(list.tail.prev == old_list.tail.prev);
    /* The head sentinel's `next` may have changed (if we inserted after head),
       so we do not assert it here. */

    /* 9. Global validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
