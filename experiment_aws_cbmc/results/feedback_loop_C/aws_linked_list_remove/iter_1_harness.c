// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_remove returns AWS_OP_SUCCESS (or the successful value):
//   - node.prev->next: CHANGES to node->next
//   - node.next->prev: CHANGES to node->prev
//   - node.next: CHANGES to NULL
//   - node.prev: CHANGES to NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_remove returns AWS_OP_ERR (or fails):
//   - node.prev->next: UNCHANGED
//   - node.next->prev: UNCHANGED
//   - node.next: UNCHANGED
//   - node.prev: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   node (struct type):
//     - next: CHANGED on success, UNCHANGED on failure
//     - prev: CHANGED on success, UNCHANGED on failure
//   node->prev (struct type):
//     - next: CHANGED on success, UNCHANGED on failure
//   node->next (struct type):
//     - prev: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_node_is_valid(node): NO (node is removed)
//   - aws_linked_list_node_is_valid(node->prev): YES
//   - aws_linked_list_node_is_valid(node->next): YES
//   - aws_linked_list_is_valid(list): YES

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_remove_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    struct aws_linked_list_node prev_node;
    struct aws_linked_list_node next_node;

    // Initialize the list and nodes
    ensure_linked_list_is_allocated(&list, 10);
    node.prev = &prev_node;
    node.next = &next_node;
    prev_node.next = &node;
    next_node.prev = &node;

    // Save old state
    struct aws_linked_list_node old_node = node;
    struct aws_linked_list_node old_prev_node = prev_node;
    struct aws_linked_list_node old_next_node = next_node;

    // Call the function under test
    aws_linked_list_remove(&node);

    // Assertions for success path
    assert(old_node.prev->next == &node ? old_node.prev->next == &old_node : old_node.prev->next == &next_node);
    assert(old_node.next->prev == &node ? old_node.next->prev == &old_node : old_node.next->prev == &prev_node);
    assert(old_node.prev->next == &next_node);
    assert(old_node.next->prev == &prev_node);
    assert(node.next == NULL);
    assert(node.prev == NULL);

    // Assertions for frame conditions
    assert(prev_node.next == &node ? prev_node.next == &old_node : prev_node.next == &next_node);
    assert(next_node.prev == &node ? next_node.prev == &old_node : next_node.prev == &prev_node);

    // Validity invariants
    assert(!aws_linked_list_node_is_valid(&node));
    assert(aws_linked_list_node_is_valid(&prev_node));
    assert(aws_linked_list_node_is_valid(&next_node));
    assert(aws_linked_list_is_valid(&list));
}
