#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_remove_harness() {
    /* 1. Set up a linked list with at least one node to remove */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    /* The list must be non-empty to remove a node */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 2. Pick a node from the list to remove.
     * We'll use the first node (head.next) as the node to remove,
     * since the list is non-empty, head.next != &list.tail */
    struct aws_linked_list_node *node = list.head.next;

    /* Ensure the node is valid for removal (not head or tail sentinel) */
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node != &list.head);
    __CPROVER_assume(node != &list.tail);
    __CPROVER_assume(node->prev != NULL);
    __CPROVER_assume(node->next != NULL);
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));

    /* 3. Save state before the call */
    struct aws_linked_list_node *prev_node = node->prev;
    struct aws_linked_list_node *next_node = node->next;

    /* 4. Call the function under test */
    aws_linked_list_remove(node);

    /* 5. Assert postconditions:
     * - prev/next of removed node point to each other
     * - node's own pointers are reset to NULL
     * - the list remains valid
     */

    /* The removed node's prev and next should be NULL (reset) */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* The surrounding nodes now point to each other */
    assert(prev_node->next == next_node);
    assert(next_node->prev == prev_node);

    /* The list should still be valid */
    assert(aws_linked_list_is_valid(&list));
}
