#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_prev_harness(void) {
    /* Allocate a node that will be passed to the function */
    struct aws_linked_list_node *node = malloc(sizeof *node);
    __CPROVER_assume(node != NULL);

    /* Allocate a possible predecessor node (may be NULL) */
    struct aws_linked_list_node *prev_candidate = malloc(sizeof *prev_candidate);
    __CPROVER_assume(prev_candidate != NULL);
    node->prev = prev_candidate;

    /* Allocate a possible successor node (may be NULL) */
    struct aws_linked_list_node *next_candidate = malloc(sizeof *next_candidate);
    __CPROVER_assume(next_candidate != NULL);
    node->next = next_candidate;

    /* Save the original state of the node */
    struct aws_linked_list_node old_node = *node;

    /* Call the function under test */
    struct aws_linked_list_node *ret = aws_linked_list_prev(node);

    /* Post‑condition: the returned pointer must be the original prev field */
    assert(ret == old_node.prev);

    /* Unchanged fields: the node itself must not be modified */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);
}
