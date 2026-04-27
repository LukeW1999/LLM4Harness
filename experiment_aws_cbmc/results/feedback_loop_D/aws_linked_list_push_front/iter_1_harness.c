// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_push_front returns AWS_OP_SUCCESS (or the successful value):
//   - list.head.next: CHANGES to point to the new node
//   - list.head.next->prev: CHANGES to point to &list.head
//   - list.tail.prev: UNCHANGED
//   - list.tail.prev->next: UNCHANGED
//   - node->next: CHANGES to point to the old front node (list.head.next)
//   - node->prev: CHANGES to point to &list.head

// === STEP 2: FAILURE PATH ===
// When aws_linked_list_push_front returns AWS_OP_ERR (or fails):
//   - list.head.next: UNCHANGED
//   - list.head.next->prev: UNCHANGED
//   - list.tail.prev: UNCHANGED
//   - list.tail.prev->next: UNCHANGED
//   - node->next: UNCHANGED
//   - node->prev: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_linked_list):
//     - head.next: CHANGED on success, UNCHANGED on failure
//     - head.prev: UNCHANGED always
//     - tail.prev: UNCHANGED always
//     - tail.next: UNCHANGED always
//   node (struct aws_linked_list_node):
//     - next: CHANGED on success, UNCHANGED on failure
//     - prev: CHANGED on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&list): YES (must hold after call)
//   - aws_linked_list_node_is_valid(&node): YES on success, NO on failure

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = node;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(node.next == NULL && node.prev == NULL);

    aws_linked_list_push_front(&list, &node);

    // Step 1: Success path assertions
    if (aws_linked_list_is_valid(&list)) {
        assert(list.head.next == &node);
        assert(node.prev == &list.head);
        assert(node.next == old_list.head.next);
        assert(list.tail.prev == old_list.tail.prev);
        assert(old_list.head.next->prev == &node);
    }

    // Step 2: Failure path assertions
    if (!aws_linked_list_is_valid(&list)) {
        assert(list.head.next == old_list.head.next);
        assert(list.tail.prev == old_list.tail.prev);
        assert(node.next == old_node.next);
        assert(node.prev == old_node.prev);
    }

    // Step 3: Frame conditions
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    // Step 4: Validity invariants
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_is_valid(&node) == aws_linked_list_is_valid(&list));
}
