#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_remove_harness() {
    /* Create a bounded, well-formed linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* Pick a node to remove; assume it is in the list and is not the head or tail sentinel */
    struct aws_linked_list_node *node;
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(aws_linked_list_node_is_in_list(node));

    /*
     * The implementation dereferences node->prev and node->next.
     * For validity, ensure both are non-NULL and correctly linked.
     * We also assume that the node's prev and next are mutually consistent
     * (node->prev->next == node && node->next->prev == node).
     * This is part of list validity, but we must also guarantee that 
     * node is not the head or tail sentinel; those are not in the list
     * (aws_linked_list_node_is_in_list excludes them in typical usage).
     */
    __CPROVER_assume(node->prev != NULL);
    __CPROVER_assume(node->next != NULL);
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node) &&
                     aws_linked_list_node_prev_is_valid(node));

    /* Save the previous and next neighbors for postcondition checks */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* Call the function */
    aws_linked_list_remove(node);

    /* --- Postconditions --- */

    /* 1. The node's next and prev are now NULL (via aws_linked_list_node_reset) */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* 2. The neighbors now skip over the removed node */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* 3.
