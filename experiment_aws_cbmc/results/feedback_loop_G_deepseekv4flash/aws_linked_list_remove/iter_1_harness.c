#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_remove_harness() {
    /* Create a list with bounded size */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume the list is valid and non-empty (so there is a node to remove) */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Pick the first node (head's next) as the node to remove */
    struct aws_linked_list_node *node = list.head.next;

    /* Save pointers that will be modified: old prev and next of node */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* Call the function under test */
    aws_linked_list_remove(node);

    /* Postconditions */

    /* 1. Node's own pointers must be set to NULL (reset) */
    assert(node->prev == NULL);
    assert(node->next == NULL);

    /* 2. The removed node's neighbors must now point to each other */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* 3. The list must still be valid (shallow check) */
    assert(aws_linked_list_is_valid(&list));

    /* 4. Immutability: the list head and tail sentinel nodes' addresses do not change */
    /* (they are embedded in the struct, so no pointer change; but their next/prev may change) */
    /* We already validated the new linkage. */
}
