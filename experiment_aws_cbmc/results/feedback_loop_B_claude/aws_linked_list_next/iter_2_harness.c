#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_next_harness(void) {
    /* 1. Declare linked list nodes */
    struct aws_linked_list_node node;
    struct aws_linked_list_node next_node;

    /* Set up node with a valid next pointer */
    node.next = &next_node;
    node.prev = NULL;

    /* Set up next_node with valid prev pointer pointing back to node */
    next_node.prev = &node;
    next_node.next = NULL;

    /* Save old state */
    struct aws_linked_list_node *old_next = node.next;
    struct aws_linked_list_node *old_prev = node.prev;

    /* 2. Preconditions */
    __CPROVER_assume(node.next != NULL);
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node.next));

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    /* 4. Assert postconditions */

    /* result is node->next */
    assert(result == node.next);
    assert(result == old_next);

    /* Unchanged fields: the function does not modify node */
    assert(node.next == old_next);
    assert(node.prev == old_prev);

    /* Validity: result is non-null */
    assert(result != NULL);

    /* The node itself remains valid */
    assert(aws_linked_list_node_next_is_valid(&node));
}
