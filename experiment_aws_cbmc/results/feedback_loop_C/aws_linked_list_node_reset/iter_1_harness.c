// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_node_reset returns AWS_OP_SUCCESS (or the successful value):
//   - node.next: CHANGES to NULL
//   - node.prev: CHANGES to NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_node_reset returns AWS_OP_ERR (or fails):
//   - node.next: UNCHANGED
//   - node.prev: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   node (struct type):
//     - next: CHANGED on success, UNCHANGED on failure
//     - prev: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_node_is_in_list(&node): NO (since node is reset and removed from any list)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node old_node = node;

    // Initialize node to some arbitrary values
    node.next = (struct aws_linked_list_node *)nondet_ptr();
    node.prev = (struct aws_linked_list_node *)nondet_ptr();

    aws_linked_list_node_reset(&node);

    // Assert success path changes
    assert(node.next == NULL);
    assert(node.prev == NULL);

    // Assert frame conditions
    if (node.next != NULL || node.prev != NULL) {
        assert(old_node.next == node.next);
        assert(old_node.prev == node.prev);
    }

    // Assert validity invariants
    assert(!aws_linked_list_node_is_in_list(&node));
}
