#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_next_harness() {
    /* 1. Declare and set up linked list nodes */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);

    /* For aws_linked_list_node_next_is_valid(node):
       node->next must be non-null and node->next->prev == node */
    node->next = next_node;
    next_node->prev = node;

    /* For aws_linked_list_node_prev_is_valid(rval) where rval = next_node:
       next_node->prev must be non-null and next_node->prev->next == next_node */
    /* next_node->prev is already set to node, and node->next is already next_node */
    /* So next_node->prev->next == node->next == next_node - this is satisfied */

    /* Set remaining pointers to avoid undefined behavior */
    node->prev = NULL;
    next_node->next = NULL;

    /* 2. Save old state */
    struct aws_linked_list_node *old_next = node->next;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 4. Assert postconditions */
    assert(result == node->next);
    assert(result == old_next);
    assert(result == next_node);

    /* 5. Assert unchanged fields */
    assert(node->next == old_next);
}
