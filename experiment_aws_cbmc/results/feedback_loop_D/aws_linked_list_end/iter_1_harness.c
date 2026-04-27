// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_end returns a value (it doesn't return an error code):
//   - list->head: UNCHANGED
//   - list->tail: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// There is no failure path for this function as it simply returns a pointer.
// The function does not modify the list or its nodes.
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_linked_list):
//     - head: UNCHANGED always
//     - tail: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&list): YES (must hold after call)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_end_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    // Save old state
    struct aws_linked_list old_list = list;

    // Call the function
    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    // Assertions for Step 1: Success Path
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    // Assertions for Step 2: Failure Path (N/A for this function)

    // Assertions for Step 3: Frame Conditions
    assert(list.head.next == old_list.head.next); // UNCHANGED
    assert(list.head.prev == old_list.head.prev); // UNCHANGED
    assert(list.tail.next == old_list.tail.next); // UNCHANGED
    assert(list.tail.prev == old_list.tail.prev); // UNCHANGED

    // Assertions for Step 4: Validity Invariants
    assert(aws_linked_list_is_valid(&list));
}
