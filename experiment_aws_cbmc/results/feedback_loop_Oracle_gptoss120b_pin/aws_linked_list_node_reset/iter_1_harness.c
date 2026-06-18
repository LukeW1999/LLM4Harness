#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_node_reset_harness(void) {
    /* Allocate the node to be reset */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* Nondeterministically initialize the node's fields */
    node->next = (struct aws_linked_list_node *)malloc(1);
    node->prev = (struct aws_linked_list_node *)malloc(1);
    __CPROVER_assume(node->next != NULL);
    __CPROVER_assume(node->prev != NULL);

    /* Allocate a second node to serve as a frame‑condition witness */
    struct aws_linked_list_node *other_node = malloc(sizeof(*other_node));
    __CPROVER_assume(other_node != NULL);
    other_node->next = (struct aws_linked_list_node *)malloc(1);
    other_node->prev = (struct aws_linked_list_node *)malloc(1);
    __CPROVER_assume(other_node->next != NULL);
    __CPROVER_assume(other_node->prev != NULL);

    /* Save a copy of the second node for later comparison */
    struct aws_linked_list_node other_node_copy = *other_node;

    /* Call the function under verification */
    aws_linked_list_node_reset(node);

    /* Post‑condition: the node must be zeroed */
    assert(node->next == NULL);
    assert(node->prev == NULL);
    assert(AWS_IS_ZEROED(*node));

    /* Frame condition: the unrelated node must remain unchanged */
    assert(other_node->next == other_node_copy.next);
    assert(other_node->prev == other_node_copy.prev);
    assert(AWS_IS_ZEROED(*other_node) == AWS_IS_ZEROED(other_node_copy));

    /* Clean up */
    free(node->next);
    free(node->prev);
    free(node);
    free(other_node->next);
    free(other_node->prev);
    free(other_node);

    return 0;
}
