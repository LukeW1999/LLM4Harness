#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_push_back_harness(void) {
    /* 1. Allocate and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a new node (non‑NULL) */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* 3. Save old state needed for post‑condition checks */
    struct aws_linked_list_node *old_last = list.tail.prev; /* previous last element (may be head sentinel) */
    struct aws_linked_list old = list;                     /* copy of list structure (sentinel fields) */

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Post‑condition assertions (function always succeeds) */
    /* The new node becomes the last element */
    assert(list.tail.prev == node);
    assert(node->next == &list.tail);
    assert(node->prev == old_last);
    assert(old_last->next == node);

    /* 6. Unchanged fields (sentinel invariants) */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);

    /* 7. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
