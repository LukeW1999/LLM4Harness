/* Contract for aws_linked_list_remove
 *
 * Preconditions:
 *   - node != NULL
 *   - node->prev != NULL
 *   - node->next != NULL
 *   - aws_linked_list_node_is_in_list(node) == true
 *   - The list containing the node satisfies aws_linked_list_is_valid_deep
 *
 * Postconditions (validity):
 *   - node->next == NULL && node->prev == NULL   (node is reset)
 *   - The list remains valid: aws_linked_list_is_valid_deep(list) == true
 *
 * Postconditions (length):
 *   - The number of elements in the list after removal is exactly one less
 *     than before removal.
 *
 * Postconditions (frame):
 *   - All other nodes in the list retain their original next and prev pointers.
 */

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

static size_t list_length(const struct aws_linked_list *list) {
    size_t cnt = 0;
    for (struct aws_linked_list_node *cur = aws_linked_list_begin(list);
         cur != aws_linked_list_end(list);
         cur = aws_linked_list_next(cur)) {
        cnt++;
    }
    return cnt;
}

void aws_linked_list_remove_harness(void) {
    /* Allocate and initialize a list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Choose a nondeterministic number of nodes (bounded) */
    size_t max_nodes = nondet_uint();
    __CPROVER_assume(max_nodes > 0);
    __CPROVER_assume(max_nodes <= 10);   /* bound to keep state space reasonable */

    struct aws_linked_list_node *nodes[10];
    for (size_t i = 0; i < max_nodes; ++i) {
        nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes[i] != NULL);
        aws_linked_list_node_reset(nodes[i]);
        aws_linked_list_push_back(&list, nodes[i]);
    }

    /* Pick a nondeterministic node to remove */
    size_t idx = nondet_uint();
    __CPROVER_assume(idx < max_nodes);
    struct aws_linked_list_node *node = nodes[idx];

    /* Snapshot the list state before removal */
    struct aws_linked_list_node *prev_snapshot[10];
    struct aws_linked_list_node *next_snapshot[10];
    for (size_t i = 0; i < max_nodes; ++i) {
        prev_snapshot[i] = nodes[i]->prev;
        next_snapshot[i] = nodes[i]->next;
    }
    size_t len_before = list_length(&list);

    /* Preconditions */
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node->prev != NULL);
    __CPROVER_assume(node->next != NULL);
    __CPROVER_assume(aws_linked_list_node_is_in_list(node) == true);
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list) == true);

    /* Call the function under test */
    aws_linked_list_remove(node);

    /* Postconditions: node is reset */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* Postconditions: list remains valid */
    assert(aws_linked_list_is_valid_deep(&list) == true);

    /* Postconditions: length decreased by one */
    size_t len_after = list_length(&list);
    assert(len_before == len_after + 1);

    /* Postconditions: other nodes unchanged */
    for (size_t i = 0; i < max_nodes; ++i) {
        if (i == idx) continue;
        assert(nodes[i]->prev == prev_snapshot[i]);
        assert(nodes[i]->next == next_snapshot[i]);
    }

    return 0;
}
