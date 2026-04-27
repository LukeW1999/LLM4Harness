// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_prev returns a non-null value (success):
//   - node.prev: RETURNED VALUE
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_prev returns a null value (failure):
//   - node.prev: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   node (struct aws_linked_list_node):
//     - next: UNCHANGED always
//     - prev: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_node_prev_is_valid(&node): YES (must hold after call)
```

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_prev_harness() {
    struct aws_linked_list_node node;
    struct aws_linked_list_node old_node = node;

    // Initialize node with arbitrary values
    node.next = (struct aws_linked_list_node *)nondet_ptr();
    node.prev = (struct aws_linked_list_node *)nondet_ptr();

    // Ensure the node is part of a valid linked list
    ensure_linked_list_is_allocated((struct aws_linked_list *)nondet_ptr(), MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list_node *result = aws_linked_list_prev(&node);

    // Check frame conditions
    assert(node.next == old_node.next);
    if (result != NULL) {
        assert(node.prev != old_node.prev);
    } else {
        assert(node.prev == old_node.prev);
    }

    // Check validity invariants
    assert(aws_linked_list_node_prev_is_valid(&node));
}
