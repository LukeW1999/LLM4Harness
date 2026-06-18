#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_prev_harness(void) {
    /* Initialize a linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate a few nodes and add them to the list */
    const size_t max_nodes = 3;
    struct aws_linked_list_node *nodes[max_nodes];
    for (size_t i = 0; i < max_nodes; ++i) {
        nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes[i] != NULL);
        aws_linked_list_node_reset(nodes[i]);
        aws_linked_list_push_back(&list, nodes[i]);
    }

    /* Choose a node from the list nondeterministically */
    size_t idx;
    __CPROVER_assume(idx < max_nodes);
    struct aws_linked_list_node *node = nodes[idx];
    __CPROVER_assume(node != NULL);

    /* Save pre-state for frame condition checks */
    struct aws_linked_list list_before;
    memcpy(&list_before, &list, sizeof(list_before));

    struct aws_linked_list_node *prev_before = node->prev;
    struct aws_linked_list_node *next_before = node->next;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* Postcondition 1: Return value correctness */
    assert(result == prev_before);

    /* Postcondition 2: No modification of the node's pointers */
    assert(node->prev == prev_before);
    assert(node->next == next_before);

    /* Postcondition 3: List structure unchanged (frame condition) */
    assert(memcmp(&list, &list_before, sizeof(list)) == 0);

    /* Optional: the list should still be valid */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* Clean up */
    for (size_t i = 0; i < max_nodes; ++i) {
        free(nodes[i]);
    }

    return 0;
}
