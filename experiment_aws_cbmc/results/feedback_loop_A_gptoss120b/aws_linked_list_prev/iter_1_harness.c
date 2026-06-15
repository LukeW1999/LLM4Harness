#include <assert.h>
#include <stddef.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_prev_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save a copy of the list before the call */
    struct aws_linked_list old_list = list;

    /* 2. Allocate a node that is (nondeterministically) part of the list */
    struct aws_linked_list_node *node;
    __CPROVER_assume(node != NULL);
    /* Assume the node is currently in a list – this matches the Doxygen contract */
    __CPROVER_assume(aws_linked_list_node_is_in_list(node));

    /* Save the node's original fields */
    struct aws_linked_list_node old_node = *node;
    struct aws_linked_list_node *old_prev = node->prev;

    /* 3. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 4. Post‑conditions */

    /* The function simply returns the previous pointer */
    assert(result == old_prev);

    /* The node itself must remain unchanged */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* The list must remain unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 5. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
}
