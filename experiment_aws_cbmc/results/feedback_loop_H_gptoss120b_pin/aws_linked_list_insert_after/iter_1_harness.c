#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_after_harness() {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node that is guaranteed to be in the list and not the tail sentinel.
       Using the head sentinel is always safe because its next is either the first
       element or the tail sentinel, both of which are valid for insertion after. */
    struct aws_linked_list_node *after = &list.head;

    /* 3. Allocate a fresh node that is not currently in any list */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    /* pre‑condition: the node is not linked */
    to_add->prev = NULL;
    to_add->next = NULL;

    /* 4. Save old state for relevant fields */
    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_next = after->next;               /* node that follows 'after' */
    struct aws_linked_list_node *old_next_prev = old_next->prev;
    struct aws_linked_list_node *old_next_next = old_next->next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    /* 5. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 6. Post‑condition: linkage of the three involved nodes */
    assert(after->prev == old_after_prev);          /* 'after' keeps its original prev */
    assert(after->next == to_add);                  /* 'after' now points forward to to_add */
    assert(to_add->prev == after);                  /* new node points back to after */
    assert(to_add->next == old_next);               /* new node points forward to old_next */
    assert(old_next->prev == to_add);               /* old_next now points back to new node */
    assert(old_next->next == old_next_next);        /* old_next's forward link unchanged */

    /* 7. Unchanged sentinel fields */
    assert(list.head.prev == old_head_prev);        /* head.prev stays NULL */
    assert(list.tail.next == old_tail_next);        /* tail.next stays NULL */

    /* 8. Global validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
