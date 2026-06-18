#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
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
     * This is a valid interior node (not head or tail sentinel). */
    struct aws_linked_list_node *node = list.head.next;
    /* node must not be the tail sentinel */
    __CPROVER_assume(node != &list.tail);
    /* Ensure the node's connections are valid */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));

    /* 3. Save the neighbors before removal */
    struct aws_linked_list_node *prev_node = node->prev;
    struct aws_linked_list_node *next_node = node->next;

    /* 4. Call the function under test */
    aws_linked_list_remove(node);

    /* 5. Assert postconditions:
     * - prev->next now points to next_node (node is bypassed)
     * - next->prev now points to prev_node (node is bypassed)
     * - node->next is NULL (reset)
     * - node->prev is NULL (reset)
     */
    assert(prev_node->next == next_node);
    assert(next_node->prev == prev_node);
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* 6. Assert the list remains valid after removal */
    assert(aws_linked_list_is_valid(&list));
}
