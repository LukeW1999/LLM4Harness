/*=== Contract for aws_linked_list_pop_back =============================

Preconditions:
  - list != NULL
  - aws_linked_list_is_valid(list) holds
  - !aws_linked_list_empty(list) (list contains at least one node)

Postconditions (validity):
  - Returned node pointer is non‑NULL and was the previous back element
  - Returned node's next == NULL && prev == NULL
  - aws_linked_list_is_valid(list) holds after the call
  - The list length is decreased by exactly one

Postconditions (frame):
  - No memory outside the list structure and its nodes is modified
==================================================================== */

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_pop_back_harness(void) {
    /* Allocate and initialize the list */
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    assert(list != NULL);
    aws_linked_list_init(list);

    /* Assume a non‑zero length for the list */
    size_t original_len = nondet_size_t();
    __CPROVER_assume(original_len > 0);

    /* Allocate nodes and push them to the back of the list */
    struct aws_linked_list_node **nodes = malloc(original_len * sizeof(struct aws_linked_list_node *));
    assert(nodes != NULL);
    for (size_t i = 0; i < original_len; ++i) {
        nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        assert(nodes[i] != NULL);
        aws_linked_list_push_back(list, nodes[i]);
    }

    /* Keep a reference to the node that should be popped */
    struct aws_linked_list_node *expected_back = nodes[original_len - 1];

    /* Verify preconditions for the function under test */
    __CPROVER_assume(!aws_linked_list_empty(list));
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Call the function */
    struct aws_linked_list_node *popped = aws_linked_list_pop_back(list);

    /* Postcondition checks */
    assert(popped == expected_back);
    assert(popped->next == NULL);
    assert(popped->prev == NULL);
    assert(aws_linked_list_is_valid(list));

    /* Verify that the list length is original_len - 1 */
    size_t new_len = 0;
    for (struct aws_linked_list_node *it = aws_linked_list_begin(list);
         it != aws_linked_list_end(list);
         it = aws_linked_list_next(it)) {
        ++new_len;
    }
    assert(new_len == original_len - 1);

    /* Clean up remaining nodes */
    for (size_t i = 0; i < new_len; ++i) {
        struct aws_linked_list_node *n = nodes[i];
        aws_linked_list_remove(n);
        free(n);
    }
    free(nodes);
    free(list);
    return 0;
}
