#include <aws/common/linked_list.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

void aws_linked_list_node_reset_harness(void) {
    /* 1. Set up the node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Initialize node with arbitrary values */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *prev_node = malloc(sizeof(struct aws_linked_list_node));
    node->next = next_node;
    node->prev = prev_node;

    /* 2. Call function under test */
    aws_linked_list_node_reset(node);

    /* 3. Assert postconditions */
    /* After reset, next and prev should point to the node itself (sentinel pattern) */
    assert(node->next == node);
    assert(node->prev == node);
}
