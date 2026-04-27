// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_init returns AWS_OP_SUCCESS (or the successful value):
//   - list.head.next: CHANGES to &list.tail
//   - list.head.prev: CHANGES to NULL
//   - list.tail.prev: CHANGES to &list.head
//   - list.tail.next: CHANGES to NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_init returns AWS_OP_ERR (or fails):
//   - list.head.next: UNCHANGED
//   - list.head.prev: UNCHANGED
//   - list.tail.prev: UNCHANGED
//   - list.tail.next: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_linked_list):
//     - head.next: CHANGED on success, UNCHANGED on failure
//     - head.prev: CHANGED on success, UNCHANGED on failure
//     - tail.prev: CHANGED on success, UNCHANGED on failure
//     - tail.next: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&list): YES (must hold after call)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_init_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list old_list = list;

    aws_linked_list_init(&list);

    // Postconditions
    assert(list.head.next == &list.tail);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);

    // Validity invariant
    assert(aws_linked_list_is_valid(&list));
}
