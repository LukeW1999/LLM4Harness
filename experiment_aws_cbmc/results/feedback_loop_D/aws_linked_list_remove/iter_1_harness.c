// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_remove returns AWS_OP_SUCCESS (or the successful value):
//   - node->prev->next: CHANGES to node->next
//   - node->next->prev: CHANGES to node->prev
//   - node->next: UNCHANGED
//   - node->prev: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_remove returns AWS_OP_ERR (or fails):
//   - node->prev->next: UNCHANGED
//   - node->next->prev: UNCHANGED
//   - node->next: UNCHANGED
//   - node->prev: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   node (struct aws_linked_list_node):
//     - next: UNCHANGED always
//     - prev: UNCHANGED always
//   node->prev (struct aws_linked_list_node):
//     - next: CHANGED on success, UNCHANGED on failure
//     - prev: UNCHANGED always
//   node->next (struct aws_linked_list_node):
//     - next: UNCHANGED always
//     - prev: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_node_is_valid(node): YES (must hold after call)
//   - aws_linked_list_node_is_valid(node->prev): YES (must hold after call)
//   - aws_linked_list_node_is_valid(node->next): YES (must hold after call)

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

    // Save old states
    struct aws_linked_list_node old_node = node;
    struct aws_linked_list_node old_prev_node = prev_node;
    struct aws_linked_list_node old_next_node = next_node;

    // Call the function under test
    aws_linked_list_remove(&node);

    // Assertions for frame conditions and validity invariants
    assert(node.next == old_node.next); // node->next: UNCHANGED always
    assert(node.prev == old_node.prev); // node->prev: UNCHANGED always

    assert(prev_node.next != old_prev_node.next); // prev_node->next: CHANGED on success
    assert(prev_node.prev == old_prev_node.prev); // prev_node->prev: UNCHANGED always

    assert(next_node.prev != old_next_node.prev); // next_node->prev: CHANGED on success
    assert(next_node.next == old_next_node.next); // next_node->next: UNCHANGED always

    // Validity invariants
    assert(aws_linked_list_node_is_valid(&node));
    assert(aws_linked_list_node_is_valid(&prev_node));
    assert(aws_linked_list_node_is_valid(&next_node));
}
