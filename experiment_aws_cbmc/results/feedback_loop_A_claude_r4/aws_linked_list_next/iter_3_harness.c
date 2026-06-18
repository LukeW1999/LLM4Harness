#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_next_harness() {
    /* 1. Declare and set up a linked list node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Create a next node that node->next points to */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);

    /* Set up the node's next pointer to point to next_node */
    node->next = next_node;

    /* For aws_linked_list_node_next_is_valid(node):
     * node->next must be non-null (done) AND node->next->prev must be non-null */
    /* next_node->prev must point back to node for the validity check */
    next_node->prev = node;

    /* node->prev can be anything non-null for safety */
    struct aws_linked_list_node *node_prev = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node_prev != NULL);
    node->prev = node_prev;

    /* next_node->next needs to be non-null for aws_linked_list_node_prev_is_valid(rval) */
    struct aws_linked_list_node *next_node_next = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node_next != NULL);
    next_node->next = next_node_next;
    /* next_node->next->prev must be non-null */
    next_node_next->prev = next_node;

    /* Assume preconditions explicitly */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));

    /* 2. Save old state */
    struct aws_linked_list_node *old_next = node->next;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 4. Assert postconditions */
    assert(result == old_next);
    assert(result == next_node);

    /* 5. Assert unchanged fields */
    assert(node->next == old_next);

    /* 6. The result is the next pointer of the node */
    assert(result == node->next);
}
