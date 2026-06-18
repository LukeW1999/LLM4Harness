#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

void aws_linked_list_pop_back_harness(void) {
    /* Initialize an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Create a nondeterministic non‑empty list */
    size_t n = nondet_uint();
    __CPROVER_assume(n > 0 && n < 5);               /* bound for CBMC */

    struct aws_linked_list_node *nodes[5];
    for (size_t i = 0; i < n; ++i) {
        nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes[i] != NULL);
        aws_linked_list_node_reset(nodes[i]);       /* start with NULL links */
        aws_linked_list_push_back(&list, nodes[i]); /* build the list */
    }

    /* Capture pre‑state of the list */
    struct aws_linked_list_node *old_back = aws_linked_list_back(&list);
    struct aws_linked_list_node *old_next[5];
    struct aws_linked_list_node *old_prev[5];
    for (size_t i = 0; i < n; ++i) {
        old_next[i] = nodes[i]->next;
        old_prev[i] = nodes[i]->prev;
    }

    /* Call the function under verification */
    struct aws_linked_list_node *removed = aws_linked_list_pop_back(&list);

    /* 1. Return value / error code correctness */
    assert(removed == old_back);
    assert(removed->next == NULL && removed->prev == NULL);

    /* 2. Output list invariants */
    assert(aws_linked_list_is_valid(&list));

    /* Length invariant: new length = old length – 1 */
    size_t new_len = 0;
    for (struct aws_linked_list_node *it = aws_linked_list_begin(&list);
         it != aws_linked_list_end(&list);
         it = aws_linked_list_next(it)) {
        ++new_len;
    }
    assert(new_len == n - 1);

    if (n == 1) {
        /* List became empty */
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        /* New back element is the former predecessor */
        struct aws_linked_list_node *new_back = aws_linked_list_back(&list);
        assert(list.tail.prev == new_back);
        assert(new_back->next == &list.tail);
        /* old_back->prev was saved in old_prev[n-1] before removal */
        assert(new_back == old_prev[n - 1]);
    }

    /* 3. Frame condition: all other nodes unchanged (except new back's next) */
    if (n > 1) {
        for (size_t i = 0; i < n - 2; ++i) {
            assert(nodes[i]->next == old_next[i]);
            assert(nodes[i]->prev == old_prev[i]);
        }
        /* The node that became the new back (nodes[n-2]) */
        assert(nodes[n - 2]->next == &list.tail);
        assert(nodes[n - 2]->prev == old_prev[n - 2]);
    } else {
        /* n == 1, no remaining nodes to check */
    }

    /* Cleanup */
    for (size_t i = 0; i < n; ++i) {
        free(nodes[i]);
    }
}
