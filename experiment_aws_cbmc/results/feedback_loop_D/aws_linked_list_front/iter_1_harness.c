// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_front returns a non-null value (success):
//   - list.head.next: RETURNED
//   - list.tail.prev: UNCHANGED
//   - list.head.prev: UNCHANGED
//   - list.tail.next: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_front returns NULL (failure):
//   - list.head.next: UNCHANGED
//   - list.tail.prev: UNCHANGED
//   - list.head.prev: UNCHANGED
//   - list.tail.next: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_linked_list):
//   - head.next: CHANGED on success, UNCHANGED on failure
//   - head.prev: UNCHANGED always
//   - tail.next: UNCHANGED always
//   - tail.prev: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&list): YES (must hold after call)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list old_list = list;

    // Ensure the list is allocated and valid
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    // Call the function under test
    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    // Assert frame conditions and validity invariants
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    if (result != NULL) {
        assert(list.head.next != old_list.head.next);
    } else {
        assert(list.head.next == old_list.head.next);
    }

    assert(aws_linked_list_is_valid(&list));
}
