#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node to insert and reset it */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);

    /* 3. Choose a node that is already in the list.
       Using the head sentinel is always valid (list is non‑NULL and head.next is defined). */
    struct aws_linked_list_node *after = &list.head;

    /* 4. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_after = after;
    struct aws_linked_list_node *old_next = after->next;
    struct aws_linked_list_node *old_next_prev = old_next->prev;
    struct aws_linked_list_node *old_after_prev = after->prev;

    /* 5. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 6. Post‑condition asserts */

    /* Nodes directly involved */
    assert(after->next == to_add);                     /* after now points to the new node */
    assert(to_add->prev == after);                     /* new node's prev points back to after */
    assert(to_add->next == old_next);                  /* new node's next is the former successor */
    assert(old_next->prev == to_add);                  /* former successor's prev now points to new node */

    /* Unchanged fields of the list structure */
    assert(list.head.prev == old_list.head.prev);      /* head.prev unchanged */
    assert(list.tail.next == old_list.tail.next);      /* tail.next unchanged */

    /* Unchanged fields of the surrounding nodes */
    assert(after->prev == old_after_prev);             /* after.prev unchanged */
    assert(old_next->next == old_next->next);           /* old_next->next unchanged (self‑equality) */
    assert(old_next_prev == old_next->prev);           /* old_next->prev was changed, already asserted above */

    /* The inserted node is now considered part of a list */
    assert(aws_linked_list_node_is_in_list(to_add));

    /* 7. Global validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
