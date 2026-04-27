// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_begin returns a value:
//   - list.head.next: RETURNED as rval
//   - list.tail: UNCHANGED
//   - list.head: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// There is no failure path as the function does not return an error code.
// The function always returns list->head.next.
//   - list.head.next: UNCHANGED
//   - list.tail: UNCHANGED
//   - list.head: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_linked_list):
//     - head.next: CHANGED on success, UNCHANGED on failure
//     - head.prev: UNCHANGED always
//     - tail.next: UNCHANGED always
//     - tail.prev: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&list): YES (must hold after call)
```

#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_begin_harness() {
    struct aws_linked_list list;
    struct aws_linked_list old_list = list;

    // Ensure the list is allocated and valid
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    // Capture the initial state of the list
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    // Call the function
    struct aws_linked_list_node *rval = aws_linked_list_begin(&list);

    // Assert frame conditions
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    // Assert validity invariants
    assert(aws_linked_list_is_valid(&list));

    // Assert return value
    assert(rval == old_head_next);
}
