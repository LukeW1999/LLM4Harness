#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_prev_harness() {
    /* 1. Declare and set up a linked list node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Create a prev node that node->prev points to */
    struct aws_linked_list_node *prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_node != NULL);

    /* Set up the node's prev pointer */
    node->prev = prev_node;

    /* aws_linked_list_node_prev_is_valid requires node->prev != NULL (already satisfied) */
    /* aws_linked_list_node_next_is_valid on the result (prev_node) requires prev_node->next != NULL */
    struct aws_linked_list_node *prev_next = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_next != NULL);
    prev_node->next = prev_next;

    /* Ensure precondition: aws_linked_list_node_prev_is_valid(node) */
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));
    /* Ensure postcondition can hold: aws_linked_list_node_next_is_valid(result) */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(prev_node));

    /* 2. Save old state */
    struct aws_linked_list_node *old_prev = node->prev;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 4. Assert postconditions */
    assert(result == old_prev);
    assert(result == prev_node);

    /* 5. Assert unchanged fields */
    assert(node->prev == old_prev);

    /* 6. Assert result is valid */
    assert(result != NULL);
}
