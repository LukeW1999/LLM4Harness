// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_prev returns a non-null value:
//   - node.prev: REMAINS THE SAME (returned value is node.prev)

// === STEP 2: FAILURE PATH ===
// When node is the head of the list (node.prev == NULL):
//   - node.prev: REMAINS THE SAME (returns NULL)

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   node (struct aws_linked_list_node):
//     - next: UNCHANGED always
//     - prev: UNCHANGED always (the function does not modify node.prev, it only reads it)

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_node_prev_is_valid(node): YES (must hold after call if node is not the head)
//   - aws_linked_list_node_is_in_list(node): YES (must hold after call if node is in a list)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_prev_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node *old_prev = node.prev;

    // Ensure the node is part of a valid linked list
    ensure_linked_list_is_allocated((struct aws_linked_list *)0, 10); // Assuming max_length of 10 for the list

    // Capture the old state of the node
    struct aws_linked_list_node old_node = node;

    // Call the function under test
    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    // Step 1: Success path assertions
    if (old_node.prev != NULL) {
        assert(result == old_node.prev);
    } else {
        assert(result == NULL);
    }

    // Step 2: Failure path assertions (no change in node.prev)
    assert(node.prev == old_node.prev);

    // Step 3: Frame condition assertions
    assert(node.next == old_node.next);
    assert(node.prev == old_node.prev);

    // Step 4: Validity invariants
    if (old_node.prev != NULL) {
        assert(aws_linked_list_node_prev_is_valid(&node));
    }
    assert(aws_linked_list_node_is_in_list(&node));
}
