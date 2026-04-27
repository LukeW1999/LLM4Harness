#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_remove_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node prev_node;
    struct aws_linked_list_node next_node;

    // Initialize nodes
    node.prev = &prev_node;
    node.next = &next_node;
    prev_node.next = &node;
    next_node.prev = &node;

    // Add assumptions for valid nodes
    __CPROVER_assume(aws_linked_list_node_is_valid(&node));
    __CPROVER_assume(aws_linked_list_node_is_valid(&prev_node));
    __CPROVER_assume(aws_linked_list_node_is_valid(&next_node));

    // Call the function under test
    int result = aws_linked_list_remove(&node);

    // Assertions for frame conditions and validity invariants
    if (result == AWS_OP_SUCCESS) {
        assert(node.next == NULL); // node->next: NULL after removal
        assert(node.prev == NULL); // node->prev: NULL after removal

        assert(prev_node.next == &next_node); // prev_node->next: points to next_node on success
        assert(prev_node.prev == old_prev_node.prev); // prev_node->prev: UNCHANGED always

        assert(next_node.prev == &prev_node); // next_node->prev: points to prev_node on success
        assert(next_node.next == old_next_node.next); // next_node->next: UNCHANGED always
    } else {
        assert(node.next == &next_node); // node->next: UNCHANGED on failure
        assert(node.prev == &prev_node); // node->prev: UNCHANGED on failure

        assert(prev_node.next == &node); // prev_node->next: UNCHANGED on failure
        assert(prev_node.prev == old_prev_node.prev); // prev_node->prev: UNCHANGED always

        assert(next_node.prev == &node); // next_node->prev: UNCHANGED on failure
        assert(next_node.next == old_next_node.next); // next_node->next: UNCHANGED always
    }

    // Validity invariants
    assert(aws_linked_list_node_is_valid(&prev_node));
    assert(aws_linked_list_node_is_valid(&next_node));
}
