#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_NODES 10

/* Nondeterministic unsigned integer */
static unsigned int nondet_uint(void);
static bool nondet_bool(void);

/* Compute the length of a linked list */
static size_t list_length(const struct aws_linked_list *list) {
    size_t cnt = 0;
    struct aws_linked_list_node *cur = list->head.next;
    while (cur != &list->tail) {
        cnt++;
        cur = cur->next;
    }
    return cnt;
}

/* Verify that a node is isolated (next and prev are NULL) */
static void assert_node_is_isolated(const struct aws_linked_list_node *node) {
    assert(node->next == NULL);
    assert(node->prev == NULL);
}

void aws_linked_list_swap_contents_harness(void) {
    /* Allocate storage for a bounded number of nodes */
    struct aws_linked_list_node *nodes = (struct aws_linked_list_node *)malloc(
        sizeof(struct aws_linked_list_node) * MAX_NODES);
    assert(nodes != NULL);

    /* Initialize all nodes to a clean state */
    for (size_t i = 0; i < MAX_NODES; ++i) {
        aws_linked_list_node_reset(&nodes[i]);
    }

    /* Create two lists */
    struct aws_linked_list a, b;
    aws_linked_list_init(&a);
    aws_linked_list_init(&b);

    /* Nondeterministically decide how many nodes go into each list */
    unsigned int len_a = nondet_uint();
    unsigned int len_b = nondet_uint();
    __CPROVER_assume(len_a + len_b <= MAX_NODES);

    /* Populate list a */
    for (unsigned int i = 0; i < len_a; ++i) {
        aws_linked_list_push_back(&a, &nodes[i]);
    }

    /* Populate list b */
    for (unsigned int i = 0; i < len_b; ++i) {
        aws_linked_list_push_back(&b, &nodes[len_a + i]);
    }

    /* Nodes that are not placed in either list must stay isolated */
    for (size_t i = len_a + len_b; i < MAX_NODES; ++i) {
        assert_node_is_isolated(&nodes[i]);
    }

    /* Record pre‑condition lengths */
    size_t pre_len_a = list_length(&a);
    size_t pre_len_b = list_length(&b);
    assert(pre_len_a == len_a);
    assert(pre_len_b == len_b);

    /* Ensure both lists are valid before the operation */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));

    /* Call the function under verification */
    aws_linked_list_swap_contents(&a, &b);

    /* Post‑condition: lengths must be swapped */
    size_t post_len_a = list_length(&a);
    size_t post_len_b = list_length(&b);
    assert(post_len_a == pre_len_b);
    assert(post_len_b == pre_len_a);

    /* Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));

    /* Frame condition: nodes that were not in either list remain isolated */
    for (size_t i = len_a + len_b; i < MAX_NODES; ++i) {
        assert_node_is_isolated(&nodes[i]);
    }

    /* Clean up */
    free(nodes);
    return 0;
}

/* Definitions for nondeterministic helpers (CBMC will treat them as nondet) */
static unsigned int nondet_uint(void) {
    unsigned int x;
    return x;
}
static bool nondet_bool(void) {
    bool b;
    return b;
}
