#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

/* CBMC nondeterministic helpers */
unsigned int nondet_uint(void);
size_t nondet_size_t(void);

void aws_linked_list_begin_harness(void) {
    /* --------------------------------------------------------------------
     * Setup: create a linked list with a nondeterministic number of nodes.
     * -------------------------------------------------------------------- */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* nondet number of nodes, bounded to keep verification tractable */
    const size_t MAX_NODES = 5;
    size_t num_nodes = nondet_size_t();
    __CPROVER_assume(num_nodes <= MAX_NODES);

    for (size_t i = 0; i < num_nodes; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_node_reset(node);
        aws_linked_list_push_back(&list, node);
    }

    /* --------------------------------------------------------------------
     * Precondition: the list must satisfy the library's structural invariant.
     * -------------------------------------------------------------------- */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* --------------------------------------------------------------------
     * Snapshot the list state for frame‑condition checking.
     * -------------------------------------------------------------------- */
    struct aws_linked_list_node *head_next_old = list.head.next;
    struct aws_linked_list_node *head_prev_old = list.head.prev;
    struct aws_linked_list_node *tail_next_old = list.tail.next;
    struct aws_linked_list_node *tail_prev_old = list.tail.prev;

    /* --------------------------------------------------------------------
     * Call the function under test.
     * -------------------------------------------------------------------- */
    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    /* --------------------------------------------------------------------
     * Postcondition 1: return value matches the specification.
     * -------------------------------------------------------------------- */
    assert(result == list.head.next);

    /* --------------------------------------------------------------------
     * Postcondition 2: return value respects list emptiness / node validity.
     * -------------------------------------------------------------------- */
    if (aws_linked_list_empty(&list)) {
        /* In an empty list the first element is the tail sentinel. */
        assert(result == &list.tail);
    } else {
        /* Non‑empty list: result must be a real node belonging to the list. */
        assert(result != NULL);
        assert(result->prev == &list.head);
        assert(aws_linked_list_node_next_is_valid(result));
    }

    /* --------------------------------------------------------------------
     * Postcondition 3: frame condition – the list structure is unchanged.
     * -------------------------------------------------------------------- */
    assert(list.head.next == head_next_old);
    assert(list.head.prev == head_prev_old);
    assert(list.tail.next == tail_next_old);
    assert(list.tail.prev == tail_prev_old);
    assert(aws_linked_list_is_valid(&list));

    return 0;
}
