// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_begin returns a value:
//   - The function does not modify the list, it only returns a pointer to the first element.
//   - Therefore, no fields change on success.

// === STEP 2: FAILURE PATH ===
// There is no failure path for this function as it does not return an error code.
// The function always succeeds and returns a pointer to the first element.

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_linked_list):
//     - head.next: UNCHANGED always
//     - head.prev: UNCHANGED always
//     - tail.next: UNCHANGED always
//     - tail.prev: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&list): YES (must hold after call)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_begin_harness() {
    struct aws_linked_list list;
    struct aws_linked_list old_list = list;

    // Ensure the list is allocated and valid
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    // Call the function under test
    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    // Assert frame conditions
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    // Assert validity invariants
    assert(aws_linked_list_is_valid(&list));
}
