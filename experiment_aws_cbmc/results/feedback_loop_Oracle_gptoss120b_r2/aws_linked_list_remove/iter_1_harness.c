#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

/* Helper to count the number of real nodes in a list (excluding head/tail sentinels) */
static size_t list_node_count(const struct aws_linked_list *list) {
    size_t cnt = 0;
    struct aws_linked_list_node *cur = aws_linked_list_begin(list);
    const struct aws_linked_list_node *end = aws_linked_list_end(list);
    while (cur != end) {
        cnt++;
        cur = aws_linked_list_next(cur);
    }
    return cnt;
}

/* Helper to snapshot a node's next/prev pointers */
static void snapshot_node(const struct aws_linked_list_node *node,
                          struct aws_linked_list_node *snap) {
    snap->next = node->next;
    snap->prev = node->prev;
}

/* Helper to compare two nodes' next/prev pointers */
static bool compare_node(const struct aws_linked_list_node *a,
                         const struct aws_linked_list_node *b) {
    return a->next == b->next && a->prev == b->prev;
}

/* Helper to snapshot the whole list (only the head/tail sentinels) */
static void snapshot_list(const struct aws_linked_list *list,
                          struct aws_linked_list *snap) {
    snap->head.next = list->head.next;
    snap->head.prev = list->head.prev;
    snap->tail.next = list->tail.next;
    snap->tail.prev = list->tail.prev;
}

/* Helper to compare two list snapshots (only head/tail sentinels) */
static bool compare_list(const struct aws_linked_list *a,
                         const struct aws_linked_list *b) {
    return a->head.next == b->head.next && a->head.prev == b->head.prev &&
           a->tail.next == b->tail.next && a->tail.prev == b->tail.prev;
}

void aws_linked_list_remove_harness(void) {
    /* Allocate allocator (default) */
    struct aws_allocator *alloc = aws_default_allocator();

    /* Initialize an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    assert(aws_linked_list_is_valid(&list));

    /* Create a nondet number of nodes (bounded to keep CBMC tractable) */
    const size_t MAX_NODES = 5;
    size_t num_nodes = nondet_uint() % (MAX_NODES + 1); /* 0..MAX_NODES */

    /* Allocate storage for nodes */
    struct aws_linked_list_node *nodes = NULL;
    if (num_nodes > 0) {
        nodes = (struct aws_linked_list_node *)malloc(num_nodes * sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes != NULL);
        for (size_t i = 0; i < num_nodes; ++i) {
            /* Ensure each node starts in a reset state */
            aws_linked_list_node_reset(&nodes[i]);
            /* Insert each node at the back of the list */
            aws_linked_list_push_back(&list, &nodes[i]);
        }
    }

    /* Preserve a snapshot of the list before removal */
    struct aws_linked_list list_before;
    snapshot_list(&list, &list_before);

    /* Snapshot each node's next/prev before removal */
    struct aws_linked_list_node *node_snapshots = NULL;
    if (num_nodes > 0) {
        node_snapshots = (struct aws_linked_list_node *)malloc(num_nodes * sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node_snapshots != NULL);
        for (size_t i = 0; i < num_nodes; ++i) {
            snapshot_node(&nodes[i], &node_snapshots[i]);
        }
    }

    /* Choose a node to remove nondeterministically (if any exist) */
    struct aws_linked_list_node *node_to_remove = NULL;
    if (num_nodes > 0) {
        size_t idx = nondet_uint() % num_nodes;
        node_to_remove = &nodes[idx];
    }

    /* Record pre‑condition counts */
    size_t count_before = list_node_count(&list);

    /* Call the function under test */
    if (node_to_remove != NULL) {
        aws_linked_list_remove(node_to_remove);
    }

    /* Post‑condition 1: the list must remain valid */
    assert(aws_linked_list_is_valid(&list));

    /* Post‑condition 2: the removed node must be reset (next and prev NULL) */
    if (node_to_remove != NULL) {
        assert(node_to_remove->next == NULL);
        assert(node_to_remove->prev == NULL);
    }

    /* Post‑condition 3: the number of nodes decreased by one (if a node was removed) */
    size_t count_after = list_node_count(&list);
    if (node_to_remove != NULL) {
        assert(count_after + 1 == count_before);
    } else {
        assert(count_after == count_before);
    }

    /* Post‑condition 4: all other nodes retain their original next/prev relationships,
       except those that were directly linked to the removed node. */
    if (num_nodes > 0) {
        for (size_t i = 0; i < num_nodes; ++i) {
            if (&nodes[i] == node_to_remove) {
                continue; /* already checked reset */
            }
            /* For nodes that were adjacent to the removed node, their pointers will have changed.
               We allow those changes; for all others, the snapshot must match. */
            bool adjacent = false;
            if (node_to_remove != NULL) {
                adjacent = (nodes[i].next == node_to_remove) || (nodes[i].prev == node_to_remove);
            }
            if (!adjacent) {
                assert(compare_node(&nodes[i], &node_snapshots[i]));
            }
        }
    }

    /* Post‑condition 5: the list sentinel nodes (head/tail) have only changed as expected.
       Their new connections must be consistent with the updated node count. */
    assert(compare_list(&list, &list_before) ||
           (node_to_remove != NULL && (
               /* head.next may have changed if the first real node was removed */
               (list.head.next == node_to_remove->next) ||
               /* tail.prev may have changed if the last real node was removed */
               (list.tail.prev == node_to_remove->prev)
           )));

    /* Clean up */
    if (nodes) free(nodes);
    if (node_snapshots) free(node_snapshots);

    return 0;
}
