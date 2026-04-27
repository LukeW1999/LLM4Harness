// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_rend returns a value (it does not return an error code):
//   - list.head: UNCHANGED
//   - list.tail: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// There is no failure path as the function does not return an error code.
// The function simply returns a pointer to the head of the list.
//   - list.head: UNCHANGED
//   - list.tail: UNCHANGED
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
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_rend_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list old_list = list;

    const struct aws_linked_list_node *result = aws_linked_list_rend(&list);

    // Step 1: Success path assertions
    assert(&list.head == result);
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    // Step 3: Frame conditions
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    // Step 4: Validity invariants
    assert(aws_linked_list_is_valid(&list));
}
