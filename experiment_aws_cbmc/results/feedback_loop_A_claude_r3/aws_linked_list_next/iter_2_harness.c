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

    /* Set up node->next to point to next_node */
    node->next = next_node;

    /* next_node->prev must point back to node for aws_linked_list_node_next_is_valid */
    next_node->prev = node;

    /* next_node->next can be anything non-null for validity */
    struct aws_linked_list_node *next_next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_next_node != NULL);
    next_node->next = next_next_node;
    next_next_node->prev = next_node;

    /* Assume precondition: aws_linked_list_node_next_is_valid(node) */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));

    /* 2. Save old state */
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node *old_prev = node->prev;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 4. Assert postconditions */
    /* The function returns node->next */
    assert(result == old_next);
    assert(result == next_node);

    /* 5. Assert unchanged fields */
    /* node itself should not be modified */
    assert(node->next == old_next);
    assert(node->prev == old_prev);
}
