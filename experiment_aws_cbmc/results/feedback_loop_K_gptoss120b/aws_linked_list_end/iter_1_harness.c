/*  CBMC harness for aws_linked_list_end  */

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

/* Helper to nondeterministically decide whether to add another node */
static bool nondet_bool(void);
static size_t nondet_size_t(void);

/* -------------------------------------------------------------------------- */
/* Harness entry point                                                         */
void aws_linked_list_end_harness(void) {
    /* --------------------------------------------------------------
       Allocate and initialise a linked list.
       -------------------------------------------------------------- */
    struct aws_linked_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);

    aws_linked_list_init(list);

    /* --------------------------------------------------------------
       Build a nondeterministically sized but bounded list.
       -------------------------------------------------------------- */
    size_t max_nodes = 5;                     /* bound to keep state space small */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= max_nodes);

    for (size_t i = 0; i < n; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(*node));
        __CPROVER_assume(node != NULL);

        /* Reset the node (sets next/prev to NULL) */
        aws_linked_list_node_reset(node);

        /* Insert the node at the back of the list */
        aws_linked_list_push_back(list, node);
    }

    /* --------------------------------------------------------------
       Ensure the list satisfies the library invariant before the call.
       -------------------------------------------------------------- */
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* --------------------------------------------------------------
       Snapshot the observable state of the list (frame condition).
       -------------------------------------------------------------- */
    struct aws_linked_list_node *head_next_before = list->head.next;
    struct aws_linked_list_node *head_prev_before = list->head.prev;
    struct aws_linked_list_node *tail_next_before = list->tail.next;
    struct aws_linked_list_node *tail_prev_before = list->tail.prev;

    /* --------------------------------------------------------------
       Call the function under test.
       -------------------------------------------------------------- */
    const struct aws_linked_list_node *retval = aws_linked_list_end(list);

    /* --------------------------------------------------------------
       Verify post‑conditions.
       -------------------------------------------------------------- */
    /* 1. Returned pointer is non‑NULL and points to the tail sentinel */
    assert(retval != NULL);
    assert(retval == &list->tail);

    /* 2. The list remains valid */
    assert(aws_linked_list_is_valid(list));

    /* 3. Frame: the list structure itself is unchanged */
    assert(list->head.next == head_next_before);
    assert(list->head.prev == head_prev_before);
    assert(list->tail.next == tail_next_before);
    assert(list->tail.prev == tail_prev_before);

    /* --------------------------------------------------------------
       Clean‑up (not required for verification but keeps tools happy).
       -------------------------------------------------------------- */
    /* Remove all nodes without freeing them – the harness does not
       need to free the individual nodes because CBMC treats the
       allocated memory as nondeterministic data. */
    aws_linked_list_move_all_back(list, &(struct aws_linked_list){0});

    free(list);
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Nondeterministic helpers (provided by CBMC)                                 */
static bool nondet_bool(void) {
    bool b;
    return b;
}
static size_t nondet_size_t(void) {
    size_t s;
    return s;
}
