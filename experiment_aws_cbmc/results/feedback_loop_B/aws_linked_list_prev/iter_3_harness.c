#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_prev_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list_node node;
    struct aws_linked_list_node prev_node;
    node.prev = &prev_node;
    node.next = NULL; // Ensure node.next is valid
    prev_node.prev = NULL; // Ensure prev_node.prev is valid
    prev_node.next = &node; // Ensure prev_node.next is valid

    /* 2. Add assumptions to satisfy preconditions */
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&node));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&prev_node));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&prev_node));

    /* 3. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list_node old_node = node;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    /* 5. Assert postconditions for BOTH success and failure paths */
    /* Success path: result should be node.prev */
    assert(result == node.prev);

    /* Failure path: not applicable for this function as it always succeeds */

    /* 6. Assert fields that must NOT change regardless of result */
    assert(node.next == old_node.next);

    /* 7. Assert validity invariant always holds */
    assert(aws_linked_list_node_prev_is_valid(&node));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_node_prev_is_valid(result));
    assert(aws_linked_list_node_next_is_valid(result));
}
