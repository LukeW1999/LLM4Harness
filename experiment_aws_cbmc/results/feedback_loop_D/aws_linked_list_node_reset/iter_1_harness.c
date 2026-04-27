// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_node_reset returns (no return value, void function):
//   - node.next: CHANGES to NULL
//   - node.prev: CHANGES to NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_node_reset does not execute due to precondition failure:
//   - node.next: UNCHANGED
//   - node.prev: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   node (struct aws_linked_list_node):
//     - next: CHANGED on success, UNCHANGED on failure
//     - prev: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_node_is_in_list(&node): NO (must not hold after call as node is removed from list)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_node_reset_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node old_node = node;

    // Simulate non-deterministic initialization
    node.next = (struct aws_linked_list_node *)nondet_ptr();
    node.prev = (struct aws_linked_list_node *)nondet_ptr();

    // Ensure the node is part of a valid list before resetting
    assume(aws_linked_list_node_is_in_list(&node));

    // Save old state
    struct aws_linked_list_node old = node;

    // Call the function under test
    aws_linked_list_node_reset(&node);

    // Assert success path changes
    assert(node.next == NULL);
    assert(node.prev == NULL);

    // Assert frame conditions
    if (old.next != NULL && old.prev != NULL) {
        // If the node was in a list, its next and prev pointers should be changed
        assert(node.next != old.next);
        assert(node.prev != old.prev);
    } else {
        // If the node was not in a list, its next and prev pointers should remain unchanged
        assert(node.next == old.next);
        assert(node.prev == old.prev);
    }

    // Assert validity invariants
    assert(!aws_linked_list_node_is_in_list(&node));
}
