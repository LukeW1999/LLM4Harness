// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_push_front returns AWS_OP_SUCCESS (or the successful value):
//   - list.head.next: CHANGES to point to the new node
//   - new_node.prev: CHANGES to point to &list.head
//   - new_node.next: CHANGES to point to the old front node
//   - old_front_node.prev: CHANGES to point to new_node
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_push_front returns AWS_OP_ERR (or fails):
//   - list.head.next: UNCHANGED
//   - new_node.prev: UNCHANGED
//   - new_node.next: UNCHANGED
//   - old_front_node.prev: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_linked_list):
//     - head.next: CHANGED on success, UNCHANGED on failure
//     - head.prev: UNCHANGED always
//     - tail.prev: UNCHANGED always
//     - tail.next: UNCHANGED always
//   new_node (struct aws_linked_list_node):
//     - prev: CHANGED on success, UNCHANGED on failure
//     - next: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&list): YES (must hold after call)
//   - aws_linked_list_node_is_valid(&new_node): YES/NO (not checked in this function)

#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node new_node;
    struct aws_linked_list_node old_front_node;

    // Initialize list and nodes with arbitrary values
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));
    old_front_node = *list.head.next;

    // Save old state
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_new_node = new_node;

    // Call the function under test
    aws_linked_list_push_front(&list, &new_node);

    // Success path assertions
    assert(list.head.next == &new_node);
    assert(new_node.prev == &list.head);
    assert(new_node.next == &old_front_node);
    assert(old_front_node.prev == &new_node);

    // Failure path assertions (not applicable here as the function does not return an error code)

    // Frame condition assertions
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.prev == old_list.tail.prev);
    assert(list.tail.next == old_list.tail.next);
    if (list.head.next == &new_node) {
        assert(new_node.prev == &list.head);
        assert(new_node.next == &old_front_node);
        assert(old_front_node.prev == &new_node);
    } else {
        assert(new_node.prev == old_new_node.prev);
        assert(new_node.next == old_new_node.next);
    }

    // Validity invariants
    assert(aws_linked_list_is_valid(&list));
}
