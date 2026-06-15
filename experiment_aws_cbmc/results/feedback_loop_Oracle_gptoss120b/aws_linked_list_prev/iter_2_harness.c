#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

/* Nondeterministic helpers for CBMC */
unsigned int nondet_uint(void);
bool nondet_bool(void);

void aws_linked_list_prev_harness(void) {
    /* Initialize a linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate a bounded number of nodes (0..4) */
    unsigned int max_nodes = nondet_uint() % 5;
    struct aws_linked_list_node *nodes[5] = {0};

    for (unsigned int i = 0; i < max_nodes; ++i) {
        nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes[i] != NULL);
        aws_linked_list_push_back(&list, nodes[i]);
    }

    /* Choose a test node: either a node from the list or a fresh isolated node */
    struct aws_linked_list_node *test_node = NULL;
    if (nondet_bool()) {
        /* Pick a node from the list if it is non‑empty */
        struct aws_linked_list_node *cur = aws_linked_list_begin(&list);
        const struct aws_linked_list_node *end = aws_linked_list_end(&list);
        if (cur != end) {
            unsigned int idx = nondet_uint() % max_nodes;
            for (unsigned int i = 0; i < idx && cur != end; ++i) {
                cur = aws_linked_list_next(cur);
            }
            test_node = cur;
        }
    } else {
        /* Fresh node not in any list */
        test_node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(test_node != NULL);
        test_node->next = NULL;
        test_node->prev = NULL;
    }

    /* Ensure we have a non‑NULL node for the call */
    __CPROVER_assume(test_node != NULL);
    /* Precondition: node's prev pointer must be valid */
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(test_node));

    /* Snapshot the entire state before the call */
    struct aws_linked_list list_before = list;
    struct aws_linked_list_node *prev_snapshot[5] = {0};
    struct aws_linked_list_node *next_snapshot[5] = {0};
    for (unsigned int i = 0; i < max_nodes; ++i) {
        prev_snapshot[i] = nodes[i]->prev;
        next_snapshot[i] = nodes[i]->next;
    }
    struct aws_linked_list_node *test_prev_before = test_node->prev;
    struct aws_linked_list_node *test_next_before = test_node->next;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(test_node);

    /* Post‑condition 1: return value matches the node's prev pointer */
    assert(result == test_prev_before);

    /* Post‑condition 2: if the returned pointer is non‑NULL, its next pointer must point back */
    if (result != NULL) {
        assert(aws_linked_list_node_next_is_valid(result));
    }

    /* Post‑condition 3: frame conditions – the list and all nodes are unchanged */
    assert(list.head.next == list_before.head.next);
    assert(list.head.prev == list_before.head.prev);
    assert(list.tail.next == list_before.tail.next);
    assert(list.tail.prev == list_before.tail.prev);

    for (unsigned int i = 0; i < max_nodes; ++i) {
        assert(nodes[i]->prev == prev_snapshot[i]);
        assert(nodes[i]->next == next_snapshot[i]);
    }

    assert(test_node->prev == test_prev_before);
    assert(test_node->next == test_next_before);

    /* Clean up allocated memory */
    for (unsigned int i = 0; i < max_nodes; ++i) {
        free(nodes[i]);
    }
    if (test_node != NULL &&
        test_node != nodes[0] && test_node != nodes[1] &&
        test_node != nodes[2] && test_node != nodes[3] && test_node != nodes[4]) {
        free(test_node);
    }

    return 0;
}
