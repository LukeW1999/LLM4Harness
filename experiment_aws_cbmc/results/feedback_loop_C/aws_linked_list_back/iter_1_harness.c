// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_back returns a non-null value (success):
//   - list.tail.prev: RETURNED VALUE
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_back returns NULL (failure):
//   - list.tail.prev: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_linked_list):
//     - head.next: UNCHANGED always
//     - head.prev: UNCHANGED always
//     - tail.next: UNCHANGED always
//     - tail.prev: RETURNED VALUE on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&list): YES (must hold after call)

#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_back_harness() {
    struct aws_linked_list list;
    size_t max_length = nondet_size_t();
    ensure_linked_list_is_allocated(&list, max_length);

    struct aws_linked_list old_list = list;

    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    // Frame conditions
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    if (result != NULL) {
        assert(list.tail.prev == result);
    } else {
        assert(list.tail.prev == old_list.tail.prev);
    }

    // Validity invariants
    assert(aws_linked_list_is_valid(&list));
}
