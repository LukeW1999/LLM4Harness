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
     * We'll use the first node (head.next) as the node to remove.
     * It must not be the tail sentinel. */
    struct aws_linked_list_node *node = list.head.next;
    /* node must be a real node (not the tail sentinel) */
    __CPROVER_assume(node != &list.tail);
    /* node must have valid prev/next pointers */
    __CPROVER_assume(node->prev != NULL);
    __CPROVER_assume(node->next != NULL);
    /* The list must satisfy deep validity (bidirectional links) */
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* 3. Save old state */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* 4. Call function under test */
    aws_linked_list_remove(node);

    /* 5. Assert postconditions */

    /* The node's prev and next should now be NULL (reset) */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* The neighbors should now point to each other, bypassing node */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* The list should still be valid */
    assert(aws_linked_list_is_valid(&list));
}
