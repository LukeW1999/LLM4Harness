// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_rbegin returns a non-null value:
//   - list.tail.prev: RETURNED by function

// === STEP 2: FAILURE PATH ===
// When aws_linked_list_rbegin returns a null value:
//   - list.tail.prev: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_linked_list):
//   - head.next: UNCHANGED always
//   - head.prev: UNCHANGED always
//   - tail.next: UNCHANGED always
//   - tail.prev: RETURNED on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&list): YES (must hold after call)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_rbegin_harness() {
    struct aws_linked_list list;
    size_t max_length = nondet_size_t();
    ensure_linked_list_is_allocated(&list, max_length);

    struct aws_linked_list old_list = list;

    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    // Check frame conditions
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    if (result != NULL) {
        assert(list.tail.prev == result);
    } else {
        assert(list.tail.prev == old_list.tail.prev);
    }

    // Check validity invariants
    assert(aws_linked_list_is_valid(&list));
}
