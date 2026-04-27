#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_prev_harness(void) {
    /* 1. Declare a linked list node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* The node must have a valid prev pointer (non-null, readable) */
    struct aws_linked_list_node *prev_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_node != NULL);

    /* Set up the node's prev pointer */
    node->prev = prev_node;

    /* Also set up prev_node's pointers to avoid undefined behavior if accessed */
    prev_node->next = node;
    prev_node->prev = malloc(sizeof(struct aws_linked_list_node));

    /* 2. Save old state */
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 4. Assert postconditions */

    /* The function returns node->prev */
    assert(result == old_prev);
    assert(result == prev_node);

    /* The function does not modify the node */
    assert(node->prev == old_prev);
    assert(node->next == old_next);

    /* The result is non-null (we assumed prev_node != NULL) */
    assert(result != NULL);

    /* The node's prev pointer is unchanged */
    assert(node->prev == prev_node);
}
