#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_prev_harness() {
    /* 1. Declare and set up a linked list node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    struct aws_linked_list_node *prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_node != NULL);

    /* Set up the node's prev pointer to point to prev_node */
    node->prev = prev_node;
    /* node->next can be anything (non-deterministic) */

    /* Save old state */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* 2. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 3. Assert postconditions */
    /* The function returns node->prev */
    assert(result == old_prev);
    assert(result == prev_node);

    /* 4. Assert unchanged fields - node itself should not be modified */
    assert(node->prev == old_prev);
    assert(node->next == old_next);
}
