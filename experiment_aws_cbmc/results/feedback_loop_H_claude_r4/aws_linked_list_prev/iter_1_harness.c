#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_prev_harness() {
    /* 1. Declare and set up a linked list node with valid prev pointer */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    struct aws_linked_list_node *prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_node != NULL);

    /* Set up bidirectional linkage so node_prev_is_valid holds */
    node->prev = prev_node;
    prev_node->next = node;

    /* Also set up prev_node's prev to something valid (non-null) */
    struct aws_linked_list_node *prev_prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_prev_node != NULL);
    prev_node->prev = prev_prev_node;
    prev_prev_node->next = prev_node;

    /* Precondition: node must have a valid prev pointer (node_prev_is_valid) */
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));

    /* 2. Save old state */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 4. Assert postconditions */
    /* The function returns node->prev */
    assert(result == old_prev);
    assert(result == node->prev);
    assert(result != NULL);

    /* 5. Assert unchanged fields - node itself is not modified */
    assert(node->prev == old_prev);
    assert(node->next == old_next);

    /* 6. Assert the returned node is the prev_node we set up */
    assert(result == prev_node);

    /* 7. Assert validity: the prev connection is still bidirectional */
    assert(aws_linked_list_node_prev_is_valid(node));
}
