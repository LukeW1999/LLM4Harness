#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node that is part of the list */
    struct aws_linked_list_node *node = list.head.next; /* could be tail if list empty */
    __CPROVER_assume(node != NULL);                     /* node must be non‑NULL */

    /* 3. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = *node;

    /* 4. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 5. Post‑conditions */

    /* 5a. Return value equals the original next pointer */
    assert(result == old_node.next);

    /* 5b. No fields of the list change */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 5c. No fields of the node change */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* 6. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
