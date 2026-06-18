#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_prev_harness() {
    /* 1. Allocate a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Pick a node to call prev on.
     *    aws_linked_list_prev is typically called on a non-head node.
     *    We need a node whose prev pointer is valid.
     *    We'll use list.tail as a concrete valid node (it always has a valid prev). */
    struct aws_linked_list_node *node = aws_linked_list_rbegin(&list);

    /* node is list.tail.prev (the last real node), or list.head if empty.
     * Either way, node->prev must be valid for the function to work correctly.
     * The precondition is that node is a valid node in the list. */
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));

    /* 3. Save old state */
    struct aws_linked_list_node *expected_prev = node->prev;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 5. Assert postconditions */
    /* The function returns node->prev */
    assert(result == expected_prev);
    assert(result == node->prev);

    /* 6. Assert that the node itself is unchanged */
    assert(node->prev == expected_prev);

    /* 7. Assert list validity is preserved */
    assert(aws_linked_list_is_valid(&list));
}
