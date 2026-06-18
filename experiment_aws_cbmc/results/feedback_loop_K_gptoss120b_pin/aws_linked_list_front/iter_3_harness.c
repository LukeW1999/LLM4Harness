/*
 * Harness for aws_linked_list_front
 * Preconditions:
 *   - list is non‑null and properly initialized.
 *   - list is non‑empty (aws_linked_list_front requires this).
 * Postconditions:
 *   - list remains valid and unchanged.
 *   - returned node is the first element in the list.
 */

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_front_harness(void) {
    /* Allocate and initialize a list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Non‑deterministically populate the list with a bounded number of nodes */
    const size_t max_nodes = 5;
    size_t num_nodes = nondet_size_t();
    __CPROVER_assume(num_nodes <= max_nodes);
    __CPROVER_assume(num_nodes > 0);               /* aws_linked_list_front precondition */

    for (size_t i = 0; i < num_nodes; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_node_reset(node);
        aws_linked_list_push_back(&list, node);
    }

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Record original pointers that must remain unchanged */
    struct aws_linked_list_node *original_head_next = list.head.next;
    struct aws_linked_list_node *original_tail_prev = list.tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *rval = aws_linked_list_front(&list);

    /* Verify that the list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* Verify that the list structure was not modified (sentinel links are unchanged) */
    assert(list.head.next == original_head_next);
    assert(list.tail.prev == original_tail_prev);
    assert(list.head.prev == &list.tail);
    assert(list.tail.next == &list.head);

    /* Verify the returned pointer (list is non‑empty) */
    assert(rval != NULL);
    assert(rval == original_head_next);
    assert(rval->prev == &list.head);
    assert(aws_linked_list_node_is_in_list(rval));

    /* Clean up allocated nodes */
    struct aws_linked_list_node *cur = aws_linked_list_begin(&list);
    while (cur != aws_linked_list_end(&list)) {
        struct aws_linked_list_node *next = aws_linked_list_next(cur);
        free(cur);
        cur = next;
    }
}
