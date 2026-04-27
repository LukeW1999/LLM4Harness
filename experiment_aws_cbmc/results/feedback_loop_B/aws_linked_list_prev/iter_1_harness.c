#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>

void aws_linked_list_prev_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list_node node;
    struct aws_linked_list_node prev_node;
    node.prev = &prev_node;

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list_node old_node = node;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Success path: result should be node.prev */
    assert(result == node.prev);

    /* Failure path: not applicable for this function as it always succeeds */

    /* 5. Assert fields that must NOT change regardless of result */
    assert(node.next == old_node.next);

    /* 6. Assert validity invariant always holds */
    /* No specific validity invariants for aws_linked_list_node in this context */
}
