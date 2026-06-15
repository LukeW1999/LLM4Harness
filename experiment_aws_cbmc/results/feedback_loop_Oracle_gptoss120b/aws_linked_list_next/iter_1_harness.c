#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_NODES 5

/* nondeterministic helpers */
bool nondet_bool(void);
size_t nondet_size_t(void);

/* Harness */
void aws_linked_list_next_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* allocate a nondeterministic number of nodes (0..MAX_NODES) */
    size_t node_count = nondet_size_t();
    __CPROVER_assume(node_count <= MAX_NODES);

    struct aws_linked_list_node *nodes[MAX_NODES];
    struct aws_linked_list_node *prev_snapshot[MAX_NODES];
    struct aws_linked_list_node *next_snapshot[MAX_NODES];

    for (size_t i = 0; i < node_count; ++i) {
        nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes[i] != NULL);
        aws_linked_list_node_reset(nodes[i]);
        aws_linked_list_push_back(&list, nodes[i]);
    }

    /* ensure the list is valid before the call */
    assert(aws_linked_list_is_valid(&list));

    /* snapshot list structure */
    struct aws_linked_list old_list = list;

    /* snapshot each node's pointers */
    for (size_t i = 0; i < node_count; ++i) {
        prev_snapshot[i] = nodes[i]->prev;
        next_snapshot[i] = nodes[i]->next;
    }

    /* pick a node from the list (if any) */
    __CPROVER_assume(node_count > 0);
    size_t idx = nondet_size_t();
    __CPROVER_assume(idx < node_count);
    struct aws_linked_list_node *node = nodes[idx];
    __CPROVER_assume(node != NULL);

    /* call the function under test */
    struct aws_linked_list_node *next = aws_linked_list_next(node);

    /* postconditions */
    /* return value must be the same as the node's next field */
    assert(next == node->next);

    /* if the returned node is non‑NULL, its prev must point back to the original node */
    if (next != NULL) {
        assert(next->prev == node);
    }

    /* the list must remain valid */
    assert(aws_linked_list_is_valid(&list));

    /* frame condition: list structure unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* frame condition: each node's pointers unchanged */
    for (size_t i = 0; i < node_count; ++i) {
        assert(nodes[i]->prev == prev_snapshot[i]);
        assert(nodes[i]->next == next_snapshot[i]);
    }

    return 0;
}
