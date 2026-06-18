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

    /* 2. Pick a node to remove — it must be an interior node (not head/tail sentinels) */
    struct aws_linked_list_node *node = aws_linked_list_begin(&list);
    /* node is list.head.next, which is a real node since list is non-empty */
    /* node != &list.tail because list is non-empty */
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node != &list.head);
    __CPROVER_assume(node != &list.tail);
    __CPROVER_assume(node->prev != NULL);
    __CPROVER_assume(node->next != NULL);

    /* Ensure the node's neighbors are valid */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));

    /* 3. Save state before the call */
    struct aws_linked_list_node *prev_node = node->prev;
    struct aws_linked_list_node *next_node = node->next;

    /* Save the prev/next of neighbors */
    struct aws_linked_list_node *prev_prev = prev_node->prev;
    struct aws_linked_list_node *next_next = next_node->next;

    /* 4. Call the function under test */
    aws_linked_list_remove(node);

    /* 5. Assert postconditions */

    /* The node's prev and next are reset to NULL after removal */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* The neighbors are now linked to each other */
    assert(prev_node->next == next_node);
    assert(next_node->prev == prev_node);

    /* The neighbors' other pointers are unchanged */
    assert(prev_node->prev == prev_prev);
    assert(next_node->next == next_next);

    /* The list itself remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* The list head and tail sentinels are unchanged */
    /* (head.prev and tail.next are always NULL in a valid list) */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
