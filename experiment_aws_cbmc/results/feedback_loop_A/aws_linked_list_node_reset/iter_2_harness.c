#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_node_reset_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list_node node;
    __CPROVER_assume(&node != NULL);

    /* 2. Initialize the node with some values */
    node.next = (struct aws_linked_list_node *)any_ptr();
    node.prev = (struct aws_linked_list_node *)any_ptr();

    /* 3. Call function under test */
    aws_linked_list_node_reset(&node);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Since the function does not return a value, we assume it always succeeds */
    assert(node.next == NULL);  // reset: node's next should be NULL
    assert(node.prev == NULL);  // reset: node's prev should be NULL

    /* 5. No other fields to check as the node is isolated */

    /* 6. No additional invariants to check for a single node reset */
}
