#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_next_harness() {
    /* 1. Allocate a linked list node non-deterministically */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* The node must have a valid next pointer (non-NULL, pointing to some node) */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);

    /* Set up the node's next pointer */
    node->next = next_node;

    /* Save old state */
    struct aws_linked_list_node *old_next = node->next;

    /* 2. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 3. Assert postconditions */
    /* The function returns node->next */
    assert(result == old_next);
    assert(result == next_node);

    /* The node itself is unchanged */
    assert(node->next == old_next);
}
