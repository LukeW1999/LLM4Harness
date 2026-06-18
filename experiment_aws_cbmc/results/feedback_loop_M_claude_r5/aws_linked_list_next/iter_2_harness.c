#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_next_harness() {
    /* 1. Allocate a linked list node non-deterministically */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* 2. Set up a valid next pointer - must be non-NULL for aws_linked_list_node_next_is_valid */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);
    node->next = next_node;

    /* 3. next_node->prev must also be valid (non-NULL) for aws_linked_list_node_prev_is_valid(rval) */
    struct aws_linked_list_node *next_node_prev = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node_prev != NULL);
    next_node->prev = next_node_prev;

    /* 4. node->prev must be valid for aws_linked_list_node_prev_is_valid */
    struct aws_linked_list_node *prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_node != NULL);
    node->prev = prev_node;

    /* 5. Assume preconditions hold */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));

    /* 6. Save old state */
    struct aws_linked_list_node *old_next = node->next;

    /* 7. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 8. Assert postconditions */
    assert(result == old_next);
    assert(result == node->next);
    assert(node->next == old_next);
}
