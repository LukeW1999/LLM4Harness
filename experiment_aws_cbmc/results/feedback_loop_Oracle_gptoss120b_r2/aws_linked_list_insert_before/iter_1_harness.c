#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

/* Maximum number of nodes that will be placed in the list before insertion */
#define MAX_INITIAL_NODES 5

/* Compute the length of a list (number of real nodes, not counting head/tail sentinels) */
static size_t list_length(const struct aws_linked_list *list) {
    size_t len = 0;
    for (struct aws_linked_list_node *cur = aws_linked_list_begin(list);
         cur != aws_linked_list_end(list);
         cur = aws_linked_list_next(cur)) {
        ++len;
    }
    return len;
}

/* Snapshot of a node's next/prev pointers for frame condition checking */
struct node_snapshot {
    struct aws_linked_list_node *next;
    struct aws_linked_list_node *prev;
};

void aws_linked_list_insert_before_harness(void) {
    /* Allocate and initialise an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* Create a nondeterministic number of initial nodes (at least 1) */
    size_t initial_cnt = nondet_uint() % (MAX_INITIAL_NODES + 1);
    if (initial_cnt == 0) {
        initial_cnt = 1; /* ensure non‑empty list for a valid `before` node */
    }

    struct aws_linked_list_node *nodes[MAX_INITIAL_NODES];
    for (size_t i = 0; i < initial_cnt; ++i) {
        nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes[i] != NULL);
        aws_linked_list_node_reset(nodes[i]);
        aws_linked_list_push_back(&list, nodes[i]);
    }

    /* Choose a nondeterministic `before` node that is already in the list */
    size_t before_idx = nondet_uint() % initial_cnt;
    struct aws_linked_list_node *before = nodes[before_idx];
    __CPROVER_assume(before != NULL);

    /* Allocate a fresh node to be inserted */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(to_add->next == NULL && to_add->prev == NULL);

    /* Snapshot of list validity and length before the operation */
    bool pre_valid = aws_linked_list_is_valid(&list);
    bool pre_valid_deep = aws_linked_list_is_valid_deep(&list);
    size_t pre_len = list_length(&list);

    /* Snapshot of all nodes' next/prev pointers for frame condition */
    struct node_snapshot snapshots[MAX_INITIAL_NODES];
    for (size_t i = 0; i < initial_cnt; ++i) {
        snapshots[i].next = nodes[i]->next;
        snapshots[i].prev = nodes[i]->prev;
    }

    /* Save the node that currently precedes `before` */
    struct aws_linked_list_node *old_before_prev = before->prev;

    /* Call the function under verification */
    aws_linked_list_insert_before(before, to_add);

    /* -------------------------------------------------------------------- */
    /* Post‑condition 1: list validity predicates */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* Post‑condition 2: length invariant (list grew by exactly one node) */
    assert(list_length(&list) == pre_len + 1);

    /* Post‑condition 3: structural relationships introduced by insertion */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(before->prev == to_add);
    assert(old_before_prev->next == to_add);

    /* -------------------------------------------------------------------- */
    /* Frame condition: nodes unrelated to the insertion remain unchanged */
    for (size_t i = 0; i < initial_cnt; ++i) {
        struct aws_linked_list_node *n = nodes[i];
        if (n != before && n != old_before_prev) {
            assert(n->next == snapshots[i].next);
            assert(n->prev == snapshots[i].prev);
        }
    }
    /* The freshly allocated node `to_add` may have its fields modified, which is
       allowed, and no other memory should be touched. */

    /* Clean up */
    for (size_t i = 0; i < initial_cnt; ++i) {
        free(nodes[i]);
    }
    free(to_add);

    return 0;
}
