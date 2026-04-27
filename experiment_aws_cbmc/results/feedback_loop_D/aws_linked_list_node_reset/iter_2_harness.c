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
    // Since aws_linked_list_node_reset always sets next and prev to NULL,
    // we do not need to check if they were changed or not.
    // The assertions above cover this.

    // Assert validity invariants
    assert(!aws_linked_list_node_is_in_list(&node));
}
