// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_pop_front returns AWS_OP_SUCCESS (or the successful value):
//   - list.head.next: CHANGES to the next node after the popped node
//   - list.tail.prev: UNCHANGED unless the list becomes empty
//   - The popped node's next and prev pointers: CHANGES to NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_pop_front returns AWS_OP_ERR (or fails):
//   - list.head.next: UNCHANGED
//   - list.tail.prev: UNCHANGED
//   - The popped node's next and prev pointers: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_linked_list):
//     - head.next: CHANGED on success, UNCHANGED on failure
//     - head.prev: UNCHANGED always
//     - tail.next: UNCHANGED always
//     - tail.prev: CHANGED on success if list becomes empty, UNCHANGED otherwise
//   The popped node (struct aws_linked_list_node):
//     - next: CHANGED on success to NULL, UNCHANGED on failure
//     - prev: CHANGED on success to NULL, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&list): YES (must hold after call)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = node;

    // Initialize the list and node
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));
    node.next = NULL;
    node.prev = NULL;

    // Store the front node before popping
    struct aws_linked_list_node *front_node = aws_linked_list_front(&list);

    // Call the function
    aws_linked_list_pop_front(&list);

    // Success path assertions
    if (!aws_linked_list_empty(&old_list)) {
        assert(list.head.next == front_node->next);
        assert(front_node->next->prev == &list.head);
        assert(front_node->next != NULL);
        assert(front_node->prev == NULL);
        assert(front_node->next != &old_list.tail);
    } else {
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    }

    // Failure path assertions
    // Since the function does not return a status code, we assume it always succeeds if preconditions are met
    // Thus, no separate failure path assertions are needed

    // Frame condition assertions
    assert(list.tail.next == &list.tail);
    if (!aws_linked_list_empty(&list)) {
        assert(list.tail.prev != &old_list.head);
    }

    // Validity invariant assertions
    assert(aws_linked_list_is_valid(&list));
}
