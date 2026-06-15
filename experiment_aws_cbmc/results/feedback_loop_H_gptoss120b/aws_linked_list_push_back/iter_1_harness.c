#include <assert.h>
#include <stdlib.h>
#include "aws/common/linked_list.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_push_back_harness() {
    /* 1. Declare and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a new node (non‑NULL) */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* 3. Save old state before the call */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Assert postconditions (success path) */
    assert(list.tail.prev == node);                 /* new last element */
    assert(node->next == &list.tail);               /* node points to tail */
    assert(node->prev == old_last);                 /* node links back to previous last */
    assert(node->prev->next == node);               /* previous last now points to node */
    assert(node->next->prev == node);               /* tail now points back to node */

    /* 6. Assert fields that must NOT change */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* 7. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
}
