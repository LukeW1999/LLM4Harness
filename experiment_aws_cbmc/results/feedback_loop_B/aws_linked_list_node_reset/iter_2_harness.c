#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_node_reset_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list_node node;
    __CPROVER_assume(&node != NULL);

    /* 2. Initialize node for testing */
    node.next = (struct aws_linked_list_node *)any_ptr();
    node.prev = (struct aws_linked_list_node *)any_ptr();

    /* 3. Call function under test */
    aws_linked_list_node_reset(&node);

    /* 4. Assert postconditions for both success and failure paths */
    /* Since the function does not return a value, we assume it always succeeds given valid input */
    assert(node.next == NULL);  // reset: next pointer should be NULL
    assert(node.prev == NULL);  // reset: prev pointer should be NULL

    /* 5. Assert fields that must NOT change regardless of result */
    /* No other fields to check in this case */

    /* 6. Assert validity invariant always holds */
    /* No additional validity checks necessary for this function */
}
