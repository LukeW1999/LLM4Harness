#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_next_harness(void) {
    /* 1. Declare a linked list node with non-deterministic next pointer */
    struct aws_linked_list_node node;
    struct aws_linked_list_node next_node;

    /* Set up node with a valid next pointer (non-deterministically either
       pointing to next_node or being some non-null pointer) */
    node.next = &next_node;
    node.prev = NULL; /* prev doesn't matter for this function */

    /* Save old state */
    struct aws_linked_list_node *old_next = node.next;
    struct aws_linked_list_node *old_prev = node.prev;

    /* 2. Precondition: node must have a valid next pointer */
    __CPROVER_assume(node.next != NULL);
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node));

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    /* 4. Assert postconditions */

    /* Changed fields: result is node->next */
    assert(result == node.next);
    assert(result == old_next);

    /* Unchanged fields: the function does not modify node */
    assert(node.next == old_next);
    assert(node.prev == old_prev);

    /* Validity: result is non-null (we assumed next is valid/non-null) */
    assert(result != NULL);

    /* The node itself remains valid */
    assert(aws_linked_list_node_next_is_valid(&node));
}
