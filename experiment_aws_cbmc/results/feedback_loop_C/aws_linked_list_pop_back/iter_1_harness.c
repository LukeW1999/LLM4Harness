// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_pop_back returns AWS_OP_SUCCESS (or the successful value):
//   - list.head.next: CHANGES to the next node of the removed node
//   - list.tail.prev: CHANGES to the previous node of the removed node
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_pop_back returns AWS_OP_ERR (or fails):
//   - list.head.next: UNCHANGED
//   - list.tail.prev: UNCHANGED
//   - list.head.prev: UNCHANGED
//   - list.tail.next: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_linked_list):
//     - head.next: CHANGED on success, UNCHANGED on failure
//     - head.prev: UNCHANGED always
//     - tail.prev: CHANGED on success, UNCHANGED on failure
//     - tail.next: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&list): YES (must hold after call)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    // Save old state
    struct aws_linked_list old_list = list;

    // Assume list is not empty for success case
    __CPROVER_assume(!aws_linked_list_empty(&list));

    // Call the function under test
    struct aws_linked_list_node *removed_node = aws_linked_list_pop_back(&list);

    // Success path assertions
    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next == old_list.head.next->next);
    assert(list.tail.prev == old_list.tail.prev->prev);
    assert(removed_node->next == NULL);
    assert(removed_node->prev == NULL);

    // Failure path assertions (not applicable here as we assume list is not empty)
    // If list were empty, aws_linked_list_pop_back would not be called, or it would be a precondition violation
}
