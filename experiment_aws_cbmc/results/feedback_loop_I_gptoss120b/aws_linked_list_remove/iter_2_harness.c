#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_NODES 5

void aws_linked_list_remove_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_NODES);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Populate the list with a nondeterministic number of nodes (at least 1) */
    size_t num_nodes = (nondet_uint32_t() % (MAX_NODES + 1));
    __CPROVER_assume(num_nodes > 0);
    struct aws_linked_list_node *nodes[MAX_NODES];

    for (size_t i = 0; i < num_nodes; ++i) {
        nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes[i] != NULL);
        nodes[i]->next = NULL;
        nodes[i]->prev = NULL;
        aws_linked_list_push_back(&list, nodes[i]);
    }

    /* 3. Choose a node that is guaranteed to be in the list */
    size_t idx = nondet_uint32_t() % num_nodes;
    struct aws_linked_list_node *node = nodes[idx];
    __CPROVER_assume(aws_linked_list_node_is_in_list(node));

    /* Save surrounding links before removal */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* 4. Call the function under test */
    aws_linked_list_remove(node);

    /* 5. Post‑condition assertions */

    /* 5.1. The removed node is reset */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* 5.2. The surrounding nodes now point to each other */
    if (old_prev != NULL && old_next != NULL) {
        assert(old_prev->next == old_next);
        assert(old_next->prev == old_prev);
    }

    /* 5.3. All other nodes remain in the list */
    for (size_t i = 0; i < num_nodes; ++i) {
        if (i != idx) {
            assert(aws_linked_list_node_is_in_list(nodes[i]));
        }
    }

    /* 5.4. List invariants are preserved */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* 5.5. The removed node is no longer in the list */
    assert(!aws_linked_list_node_is_in_list(node));
}
