// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_next returns a non-null value:
//   - node.next: RETURNED
//   - node.prev: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_next returns a null value:
//   - node.next: UNCHANGED
//   - node.prev: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   node (struct aws_linked_list_node type):
//     - next: CHANGED on success, UNCHANGED on failure
//     - prev: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_node_next_is_valid(&node): YES (must hold after call)
```

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_next_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node old_node = node;

    // Initialize node with non-deterministic values
    node.next = (struct aws_linked_list_node *)nondet_ptr();
    node.prev = (struct aws_linked_list_node *)nondet_ptr();

    // Ensure the node is part of a valid linked list
    ensure_linked_list_is_allocated((struct aws_linked_list *)nondet_ptr(), MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    // Success path assertions
    if (result != NULL) {
        assert(node.next == result);
        assert(node.prev == old_node.prev);
        assert(aws_linked_list_node_next_is_valid(&node));
    }
    // Failure path assertions
    else {
        assert(node.next == old_node.next);
        assert(node.prev == old_node.prev);
        assert(aws_linked_list_node_next_is_valid(&node));
    }
}
