#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/* CBMC nondeterministic helpers */
unsigned int nondet_uint(void);
void *nondet_ptr(void);

void aws_linked_list_back_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Create a nondeterministic number of nodes (0..5) and push them to the list */
    unsigned int max_nodes = nondet_uint();
    __CPROVER_assume(max_nodes <= 5);

    unsigned int node_count = nondet_uint();
    __CPROVER_assume(node_count <= max_nodes);

    for (unsigned int i = 0; i < node_count; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof *node);
        __CPROVER_assume(node != NULL);
        aws_linked_list_node_reset(node);
        aws_linked_list_push_back(&list, node);
    }

    /* Ground‑truth precondition */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Snapshot of the list structure for frame condition */
    struct aws_linked_list list_old = list;

    /* Call the function under verification */
    struct aws_linked_list_node *back = aws_linked_list_back(&list);

    /* ---------- Post‑condition asserts ---------- */

    /* 1. Return value correctness */
    assert(back != NULL);                                 /* non‑NULL because list is non‑empty */
    assert(back == list.tail.prev);                       /* must be the node before the tail */
    assert(back->next == &list.tail);                     /* its next pointer must point to tail */
    assert(aws_linked_list_node_next_is_valid(back));      /* bidirectional next link is valid */
    assert(aws_linked_list_node_prev_is_valid(back));      /* bidirectional prev link is valid */

    /* 2. No output buffer length/capacity invariants needed for this function */

    /* 3. Frame condition – the list structure must be unchanged */
    assert(memcmp(&list, &list_old, sizeof(list)) == 0);

    return 0;
}
