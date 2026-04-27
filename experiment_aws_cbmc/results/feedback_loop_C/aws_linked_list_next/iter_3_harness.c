#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_next_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node old_node = node;

    // Ensure the node is part of a valid linked list
    ensure_linked_list_is_allocated(NULL, 1); // We don't have a full list, just ensuring the node is valid
    node.next = (struct aws_linked_list_node *)nondet_ptr();
    node.prev = (struct aws_linked_list_node *)nondet_ptr();

    // Assume the node is valid before calling the function
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&node));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(&node));

    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    // Step 3 frame condition asserts
    if (result != NULL) {
        assert(node.next == result);
    }

    // Step 4 validity invariant asserts
    if (node.next != NULL) {
        assert(aws_linked_list_node_next_is_valid(&node));
    }
}
