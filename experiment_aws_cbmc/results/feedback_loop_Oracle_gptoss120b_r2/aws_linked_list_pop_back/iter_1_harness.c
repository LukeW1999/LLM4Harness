```c#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_NODES 5

size_t nondet_size_t(void);
bool nondet_bool(void);

void aws_linked_list_pop_back_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Choose a non‑zero length for the list */
    size_t len = nondet_size_t();
    __CPROVER_assume(len >= 1 && len <= MAX_NODES);

    struct aws_linked_list_node nodes[MAX_NODES];
    struct aws_linked_list_node nodes_before[MAX_NODES];

    /* Initialise nodes and build the list */
    for (size_t i = 0; i < len; ++i) {
        aws_linked_list_node_reset(&nodes[i]);
        aws_linked_list_push_back(&list, &nodes[i]);
    }

    /* Remember the state before the call */
    struct aws_linked_list list_before = list;
    for (size_t i = 0; i < len; ++i) {
        nodes_before[i] = nodes[i];
    }

    /* The node that should be returned */
    struct aws_linked_list_node *expected_back = &nodes[len - 1];

    /* Call the function under test */
    struct aws_linked_list_node *popped = aws_linked_list_pop_back(&list);

    /* 1. Return value correctness */
    assert(popped == expected_back);
    assert(popped->next == NULL);
    assert(popped->prev == NULL);

    /* 2. Length / structural invariants */
    if (len == 1) {
        /* List must be empty now */
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        /* New tail should be the previous node */
        assert(list.tail.prev == &nodes[len - 2]);
        /* Head should still point to the first node */
        assert(list.head.next == &nodes[0]);
    }
    /* List must still be valid */
    assert(aws_linked_list_is_valid(&list));

    /* 3. Frame conditions – nodes other than the popped one are unchanged */
    for (size_t i = 0; i < len - 1; ++i) {
        assert(nodes[i].next == nodes_before[i].next);
        assert(nodes[i].prev == nodes_before[i].prev);
    }

    /* The list structure fields that are not supposed to change remain unchanged */
    assert(list_before.head.prev == list.head.prev);   /* should be NULL */
    assert(list_before.tail.next == list.tail.next);   /* should be NULL */

    return 0;
}
