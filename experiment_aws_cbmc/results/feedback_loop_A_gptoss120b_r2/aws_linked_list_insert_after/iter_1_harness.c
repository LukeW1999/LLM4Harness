#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node that is guaranteed to be in the list.
       The head sentinel is always present, so we use it as the
       insertion point. */
    struct aws_linked_list_node *after = &list.head;

    /* 3. Allocate a node that is not currently in any list */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    /* make the node look like a fresh node */
    to_add->next = NULL;
    to_add->prev = NULL;

    /* 4. Save old state needed for post‑condition checks */
    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_next_next = old_after_next->next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    /* 5. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 6. Assert the fields that must have changed */
    assert(after->next == to_add);                     /* after now points to the new node */
    assert(to_add->prev == after);                     /* new node's prev points back to after */
    assert(to_add->next == old_after_next);            /* new node's next is the former successor */
    assert(old_after_next->prev == to_add);            /* former successor's prev now points to new node */

    /* 7. Assert fields that must remain unchanged */
    assert(after->prev == old_after_prev);             /* after's prev unchanged */
    assert(old_after_next->next == old_after_next_next);/* former successor's next unchanged */
    assert(list.head.prev == old_head_prev);           /* list head's prev unchanged (should be NULL) */
    assert(list.tail.next == old_tail_next);           /* list tail's next unchanged (should be NULL) */

    /* 8. The new node should now be considered part of a list */
    assert(aws_linked_list_node_is_in_list(to_add));

    /* 9. Global validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
