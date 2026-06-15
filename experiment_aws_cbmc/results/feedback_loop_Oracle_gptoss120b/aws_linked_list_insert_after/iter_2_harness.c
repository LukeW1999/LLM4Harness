#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_NODES 5

/* nondet helpers */
size_t nondet_size_t(void);
bool nondet_bool(void);

/* compute list length (excluding sentinels) */
static size_t list_length(const struct aws_linked_list *list) {
    size_t cnt = 0;
    struct aws_linked_list_node *cur = aws_linked_list_begin(list);
    const struct aws_linked_list_node *end = aws_linked_list_end(list);
    while (cur != end) {
        cnt++;
        cur = aws_linked_list_next(cur);
    }
    return cnt;
}

/* snapshot of a node's links */
struct node_snapshot {
    struct aws_linked_list_node *next;
    struct aws_linked_list_node *prev;
};

void aws_linked_list_insert_after_harness(void) {
    /* --------------------------------------------------------------------
     * Build an arbitrary list (may be empty) and a node to insert.
     * -------------------------------------------------------------------- */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    size_t init_len = nondet_size_t();
    __CPROVER_assume(init_len <= MAX_NODES);

    struct aws_linked_list_node *nodes[MAX_NODES];
    size_t i;
    for (i = 0; i < init_len; ++i) {
        nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes[i] != NULL);
        aws_linked_list_node_reset(nodes[i]);
        aws_linked_list_push_back(&list, nodes[i]);
    }

    /* --------------------------------------------------------------------
     * Choose a valid 'after' node.
     *   - it may be the head sentinel (inserting at the front)
     *   - or any existing element in the list
     * -------------------------------------------------------------------- */
    struct aws_linked_list_node *after;
    if (init_len == 0) {
        after = &list.head;
    } else {
        size_t idx = nondet_size_t();
        __CPROVER_assume(idx <= init_len);               /* idx == init_len => choose head */
        if (idx == init_len) {
            after = &list.head;
        } else {
            after = nodes[idx];
        }
    }

    /* --------------------------------------------------------------------
     * Allocate the node to be added and ensure it is not linked.
     * -------------------------------------------------------------------- */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(to_add->next == NULL && to_add->prev == NULL);

    /* --------------------------------------------------------------------
     * Record pre‑state information needed for post‑conditions.
     * -------------------------------------------------------------------- */
    size_t len_before = list_length(&list);
    struct aws_linked_list_node *old_after_next = after->next;

    struct node_snapshot snap_before[MAX_NODES + 2];   /* +2 for possible head/tail */
    /* snapshot head and tail sentinels */
    snap_before[0].next = list.head.next;
    snap_before[0].prev = list.head.prev;
    snap_before[1].next = list.tail.next;
    snap_before[1].prev = list.tail.prev;
    /* snapshot all regular nodes */
    for (i = 0; i < init_len; ++i) {
        snap_before[i + 2].next = nodes[i]->next;
        snap_before[i + 2].prev = nodes[i]->prev;
    }

    /* --------------------------------------------------------------------
     * Call the function under verification.
     * -------------------------------------------------------------------- */
    aws_linked_list_insert_after(after, to_add);

    /* --------------------------------------------------------------------
     * Post‑conditions.
     * -------------------------------------------------------------------- */
    /* 1. List validity predicates */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* 2. Length invariant: length increased by exactly one */
    assert(list_length(&list) == len_before + 1);

    /* 3. Structural relationships introduced by the insertion */
    assert(to_add->prev == after);
    assert(to_add->next == old_after_next);
    assert(after->next == to_add);
    assert(old_after_next->prev == to_add);

    /* 4. Frame condition: only the three nodes involved may have changed */
    /* regular nodes other than 'after' and 'old_after_next' must be unchanged */
    for (i = 0; i < init_len; ++i) {
        struct aws_linked_list_node *n = nodes[i];
        if (n != after && n != old_after_next) {
            assert(n->next == snap_before[i + 2].next);
            assert(n->prev == snap_before[i + 2].prev);
        }
    }
}
