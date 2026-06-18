#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_prev_harness(void) {
    /* Initialize an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Build a nondeterministic list of up to 5 nodes */
    const size_t max_nodes = 5;
    size_t num_nodes = nondet_size_t();
    __CPROVER_assume(num_nodes <= max_nodes);

    for (size_t i = 0; i < num_nodes; ++i) {
        struct aws_linked_list_node *n = malloc(sizeof(*n));
        __CPROVER_assume(n != NULL);
        aws_linked_list_node_reset(n);
        aws_linked_list_push_back(&list, n);
    }

    /* Ensure the list is still valid after construction */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Choose a node from the list (excluding the head sentinel) */
    struct aws_linked_list_node *node = NULL;
    {
        /* nondet choice: either pick the tail sentinel,
           or a real element from the list */
        bool pick_tail = nondet_bool();
        if (pick_tail) {
            node = &list.tail;
        } else {
            /* pick a real element */
            struct aws_linked_list_node *it = aws_linked_list_begin(&list);
            size_t idx = nondet_size_t();
            __CPROVER_assume(idx < num_nodes);
            for (size_t i = 0; i < idx; ++i) {
                it = aws_linked_list_next(it);
            }
            node = it;
        }
    }

    /* Preconditions: node must be a valid pointer and not the head sentinel */
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node != &list.head);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Snapshot of the node's fields for frame condition */
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node *old_prev = node->prev;

    /* Call the function under verification */
    struct aws_linked_list_node *prev = aws_linked_list_prev(node);

    /* Postcondition 1: return value equals node->prev (captured before call) */
    assert(prev == old_prev);

    /* Postcondition 2: the list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition 3: frame condition – node's next/prev unchanged */
    assert(node->next == old_next);
    assert(node->prev == old_prev);
}
