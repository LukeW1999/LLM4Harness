// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_push_back returns AWS_OP_SUCCESS (or the successful value):
//   - list->head.next: CHANGES to node
//   - list->tail.prev: CHANGES to node
//   - node->next: CHANGES to &list->tail
//   - node->prev: CHANGES to old list->tail.prev
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_push_back returns AWS_OP_ERR (or fails):
//   - list->head.next: UNCHANGED
//   - list->tail.prev: UNCHANGED
//   - node->next: UNCHANGED
//   - node->prev: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_linked_list):
//     - head.next: CHANGED on success, UNCHANGED on failure
//     - head.prev: UNCHANGED always
//     - tail.next: UNCHANGED always
//     - tail.prev: CHANGES on success, UNCHANGED on failure
//   node (struct aws_linked_list_node):
//     - next: CHANGES on success, UNCHANGED on failure
//     - prev: CHANGES on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&list): YES (must hold after call)
//   - aws_linked_list_node_next_is_valid(node): YES on success, NO on failure
//   - aws_linked_list_node_prev_is_valid(node): YES on success, NO on failure
```

#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    // Initialize the list and node with non-deterministic values
    ensure_linked_list_is_allocated(&list, 10);
    node.next = NULL;
    node.prev = NULL;

    // Save old state
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = node;

    // Call the function under test
    aws_linked_list_push_back(&list, &node);

    // Assertions based on the analysis
    assert(aws_linked_list_is_valid(&list));

    // Success path assertions
    assert(list.head.next == (old_list.tail.prev == &old_list.head ? &node : old_list.head.next));
    assert(list.tail.prev == &node);
    assert(node.next == &list.tail);
    assert(node.prev == (old_list.tail.prev == &old_list.head ? &list.head : old_list.tail.prev));

    // Failure path assertions (these should not change the state)
    if (old_list.tail.prev != &old_list.head) {
        assert(list.head.next == old_list.head.next);
        assert(list.tail.prev == old_list.tail.prev);
        assert(node.next == old_node.next);
        assert(node.prev == old_node.prev);
    }

    // Frame conditions
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
}
