#include <assert.h>
#include <stddef.h>

#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

/* Nondeterministic size generator for CBMC */
size_t nondet_size_t(void);

static size_t list_length(const struct aws_linked_list *list) {
    size_t len = 0;
    for (struct aws_linked_list_node *cur = list->head.next; cur != &list->tail; cur = cur->next) {
        len++;
    }
    return len;
}

void aws_linked_list_pop_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Create a non‑empty list: at least one node, bounded for verification */
    size_t n = nondet_size_t();
    __CPROVER_assume(n > 0 && n < 10);

    /* Stack‑allocated array of nodes (maximum size) */
    struct aws_linked_list_node nodes[10];

    /* Populate the list by pushing nodes to the front */
    for (size_t i = 0; i < n; ++i) {
        aws_linked_list_node_reset(&nodes[i]);
        aws_linked_list_push_front(&list, &nodes[i]);
    }

    /* Record pre‑state information */
    size_t len_before = list_length(&list);
    struct aws_linked_list_node *expected_front = list.head.next;

    /* Call the function under test */
    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    /* Post‑conditions */

    /* 1. Return value correctness */
    assert(popped == expected_front);
    assert(popped->next == NULL);
    assert(popped->prev == NULL);

    /* 2. List validity and length invariants */
    assert(aws_linked_list_is_valid(&list));
    size_t len_after = list_length(&list);
    assert(len_after + 1 == len_before);

    /* 3. Frame conditions: remaining nodes stay correctly linked */
    for (struct aws_linked_list_node *cur = list.head.next; cur != &list.tail; cur = cur->next) {
        assert(cur->next->prev == cur);
        assert(cur->prev->next == cur);
    }
}
