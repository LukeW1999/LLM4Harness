#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_next_harness() {
    /* 1. Declare and set up a linked list node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Create a next node that node->next points to */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);

    /* Create a prev node for next_node so aws_linked_list_node_prev_is_valid(next_node) holds */
    struct aws_linked_list_node *next_node_prev = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node_prev != NULL);

    /* Set up node->next to point to next_node */
    node->next = next_node;

    /* next_node->prev must be valid (non-null) for the postcondition check */
    next_node->prev = next_node_prev;

    /* Assume precondition: aws_linked_list_node_next_is_valid(node) */
    /* This means node->next != NULL, which we've already set */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));

    /* Also assume the postcondition on the result will hold */
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(next_node));

    /* 2. Save old state */
    struct aws_linked_list_node *old_next = node->next;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 4. Assert postconditions */
    /* The function returns node->next */
    assert(result == old_next);
    assert(result == next_node);

    /* 5. Assert unchanged fields */
    /* node->next should not have been modified */
    assert(node->next == old_next);
}
