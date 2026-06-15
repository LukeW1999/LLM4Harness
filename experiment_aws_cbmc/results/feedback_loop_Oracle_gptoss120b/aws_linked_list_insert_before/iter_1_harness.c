#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_NODES 5

/* Helper to nondeterministically decide a boolean */
static bool nondet_bool(void);
static size_t nondet_size_t(void);

/* Snapshot of a node's connections */
struct node_snapshot {
    struct aws_linked_list_node *node;
    struct aws_linked_list_node *next;
    struct aws_linked_list_node *prev;
};

void aws_linked_list_insert_before_harness(void) {
    /* Allocate and initialize a list */
    struct aws_linked_list *list = malloc(sizeof *list);
    assert(list != NULL);
    aws_linked_list_init(list);

    /* Create a nondet number of nodes (0..MAX_NODES) and push them */
    size_t pre_len = nondet_size_t();
    __CPROVER_assume(pre_len <= MAX_NODES);

    /* Ensure at least one node so that we have a valid 'before' */
    __CPROVER_assume(pre_len >= 1);

    struct aws_linked_list_node *nodes[MAX_NODES];
    for (size_t i = 0; i < pre_len; ++i) {
        nodes[i] = malloc(sizeof *nodes[i]);
        assert(nodes[i] != NULL);
        aws_linked_list_node_reset(nodes[i]);
        aws_linked_list_push_back(list, nodes[i]);
    }

    /* Choose a nondet index for the 'before' node */
    size_t before_idx = nondet_size_t();
    __CPROVER_assume(before_idx < pre_len);
    struct aws_linked_list_node *before = nodes[before_idx];

    /* Allocate the node to add */
    struct aws_linked_list_node *to_add = malloc(sizeof *to_add);
    assert(to_add != NULL);
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(to_add->next == NULL && to_add->prev == NULL);

    /* Snapshot of list length and node connections before insertion */
    size_t old_len = 0;
    struct aws_linked_list_node *iter = aws_linked_list_begin(list);
    while (iter != aws_linked_list_end(list)) {
        ++old_len;
        iter = aws_linked_list_next(iter);
    }
    __CPROVER_assume(old_len == pre_len); /* sanity */

    struct node_snapshot snapshots[MAX_NODES];
    size_t snap_cnt = 0;
    iter = aws_linked_list_begin(list);
    while (iter != aws_linked_list_end(list)) {
        snapshots[snap_cnt].node = iter;
        snapshots[snap_cnt].next = iter->next;
        snapshots[snap_cnt].prev = iter->prev;
        ++snap_cnt;
        iter = aws_linked_list_next(iter);
    }

    /* Remember the old predecessor of 'before' */
    struct aws_linked_list_node *old_prev = before->prev;

    /* Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* ---- Postconditions ---- */

    /* 1. List validity predicates */
    assert(aws_linked_list_is_valid(list));
    assert(aws_linked_list_is_valid_deep(list));

    /* 2. Length invariant: length increased by exactly one */
    size_t new_len = 0;
    iter = aws_linked_list_begin(list);
    while (iter != aws_linked_list_end(list)) {
        ++new_len;
        iter = aws_linked_list_next(iter);
    }
    assert(new_len == old_len + 1);

    /* 3. Node connections for the inserted node and its neighbours */
    assert(to_add->next == before);
    assert(to_add->prev == old_prev);
    assert(before->prev == to_add);
    assert(old_prev->next == to_add);
    assert(aws_linked_list_node_is_in_list(to_add));

    /* 4. Frame condition: all other nodes' connections unchanged */
    for (size_t i = 0; i < snap_cnt; ++i) {
        struct aws_linked_list_node *n = snapshots[i].node;
        if (n == before || n == to_add || n == old_prev) {
            continue;
        }
        assert(n->next == snapshots[i].next);
        assert(n->prev == snapshots[i].prev);
    }

    /* 5. No other memory modifications (sentinel nodes) */
    assert(list->head.prev == &list->head);
    assert(list->tail.next == &list->tail);

    return 0;
}

/* Nondeterministic helpers (CBMC builtins) */
static bool nondet_bool(void) {
    bool b;
    return b;
}
static size_t nondet_size_t(void) {
    size_t s;
    return s;
}
