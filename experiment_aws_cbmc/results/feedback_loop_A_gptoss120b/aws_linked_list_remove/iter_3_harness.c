#include <stdlib.h>
#include <assert.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_remove_harness(void) {
    /* initialize an empty list and assume it is valid */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* allocate two nodes and insert them into the list */
    struct aws_linked_list_node *n1 = malloc(sizeof(*n1));
    struct aws_linked_list_node *n2 = malloc(sizeof(*n2));
    __CPROVER_assume(n1 != NULL && n2 != NULL);
    aws_linked_list_node_reset(n1);
    aws_linked_list_node_reset(n2);
    aws_linked_list_push_back(&list, n1);
    aws_linked_list_push_back(&list, n2);

    /* nondeterministically choose one of the nodes to remove */
    struct aws_linked_list_node *node = __CPROVER_nondet_int() ? n1 : n2;

    /* save the neighboring pointers before removal */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* call the function under test */
    aws_linked_list_remove(node);

    /* post‑condition checks */
    assert(node->next == NULL);
    assert(node->prev == NULL);
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);
    assert(!aws_linked_list_node_is_in_list(node));
    assert(aws_linked_list_is_valid(&list));
}
