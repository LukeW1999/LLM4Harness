#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_NODES 5

static size_t list_length(const struct aws_linked_list *list) {
    size_t len = 0;
    struct aws_linked_list_node *cur = list->head.next;
    while (cur != &list->tail) {
        len++;
        cur = cur->next;
    }
    return len;
}

void aws_linked_list_pop_front_harness(void) {
    /* Initialize an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    assert(aws_linked_list_is_valid(&list));

    /* Nondeterministically decide the number of nodes (must be >0 for the precondition) */
    size_t n;
    __CPROVER_assume(n > 0 && n < MAX_NODES);

    /* Allocate storage for the nodes */
    struct aws_linked_list_node *nodes = malloc(n * sizeof(struct aws_linked_list_node));
    __CPROVER_assume(nodes != NULL);

    /* Allocate storage for snapshots of each node's pointers (frame condition) */
    struct {
        struct aws_linked_list_node *next;
        struct aws_linked_list_node *prev;
    } *snapshots = malloc(n * sizeof(*snapshots));
    __CPROVER_assume(snapshots != NULL);

    /* Build the list by pushing each node to the front */
    for (size_t i = 0; i < n; ++i) {
        aws_linked_list_node_reset(&nodes[i]);
        aws_linked_list_push_front(&list, &nodes[i]);
    }

    /* Capture pre‑state */
    size_t pre_len = list_length(&list);
    struct aws_linked_list_node *pre_head = list.head.next;
    struct aws_linked_list_node *pre_tail = list.tail.prev;
    for (size_t i = 0; i < n; ++i) {
        snapshots[i].next = nodes[i].next;
        snapshots[i].prev = nodes[i].prev;
    }

    /* Call the function under verification */
    struct aws_linked_list_node *ret = aws_linked_list_pop_front(&list);

    /* Post‑conditions */

    /* 1. Return value correctness */
    assert(ret != NULL);
    assert(ret->next == NULL && ret->prev == NULL);

    /* 2. List validity and length invariants */
    assert(aws_linked_list_is_valid(&list));
    size_t post_len = list_length(&list);
    assert(post_len + 1 == pre_len);

    if (pre_len > 1) {
        /* The new head should be the former second element */
        assert(list.head.next == pre_head->next);
    } else {
        /* List should now be empty */
        assert(list.head.next == &list.tail);
    }
    /* Tail should remain unchanged */
    assert(list.tail.prev == pre_tail);

    /* 3. Frame condition: only the popped node may have been modified */
    for (size_t i = 0; i < n; ++i) {
        if (&nodes[i] != ret) {
            assert(nodes[i].next == snapshots[i].next);
            assert(nodes[i].prev == snapshots[i].prev);
        }
    }

    /* Clean up */
    free(snapshots);
    free(nodes);
    return 0;
}
