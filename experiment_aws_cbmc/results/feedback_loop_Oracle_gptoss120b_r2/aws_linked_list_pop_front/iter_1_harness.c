#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_NODES 8

/* nondeterministic size */
size_t nondet_size_t(void);

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
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* create a non‑empty list */
    size_t n = nondet_size_t();
    __CPROVER_assume(n > 0 && n <= MAX_NODES);

    struct aws_linked_list_node *nodes[MAX_NODES];

    for (size_t i = 0; i < n; ++i) {
        nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes[i] != NULL);
        aws_linked_list_node_reset(nodes[i]);
        /* build the list by pushing to the front – order is preserved */
        aws_linked_list_push_front(&list, nodes[i]);
    }

    /* ensure all node pointers are distinct */
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            __CPROVER_assume(nodes[i] != nodes[j]);
        }
    }

    /* capture pre‑state */
    size_t len_before = list_length(&list);
    struct aws_linked_list_node *front_before = aws_linked_list_front(&list);

    /* call the function under test */
    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    /* post‑conditions */

    /* 1. return value correctness */
    assert(popped == front_before);
    assert(popped->next == NULL && popped->prev == NULL);

    /* 2. length invariant */
    size_t len_after = list_length(&list);
    assert(len_before == len_after + 1);

    /* 3. list validity */
    assert(aws_linked_list_is_valid(&list));

    /* 4. frame condition – nodes other than the popped one remain linked correctly */
    for (size_t i = 0; i < n; ++i) {
        if (nodes[i] != popped) {
            /* each remaining node must still be part of a valid list */
            assert(aws_linked_list_node_is_in_list(nodes[i]) ||
                   (nodes[i]->next == &list.tail && nodes[i]->prev == &list.head));
        }
    }

    /* clean up */
    for (size_t i = 0; i < n; ++i) {
        if (nodes[i] != popped) {
            free(nodes[i]);
        }
    }
    free(popped);

    return 0;
}
