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
    /* The list must be non-empty so we have a node to remove */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 2. Pick a node from the list to remove.
     * We'll use the front node (head.next) as the node to remove.
     * It must not be the tail sentinel. */
    struct aws_linked_list_node *node = list.head.next;
    /* node is a real node (not the tail sentinel) */
    __CPROVER_assume(node != &list.tail);
    /* Ensure node's prev and next are valid (non-null) */
    __CPROVER_assume(node->prev != NULL);
    __CPROVER_assume(node->next != NULL);

    /* Save the neighbors before removal */
    struct aws_linked_list_node *prev_node = node->prev;
    struct aws_linked_list_node *next_node = node->next;

    /* 3. Call the function under test */
    aws_linked_list_remove(node);

    /* 4. Assert postconditions */

    /* The prev and next of the removed node's neighbors now point to each other */
    assert(prev_node->next == next_node);
    assert(next_node->prev == prev_node);

    /* The removed node's pointers are reset to NULL */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* 5. Assert the list remains valid */
    assert(aws_linked_list_is_valid(&list));
}
