// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_swap_contents returns AWS_OP_SUCCESS (or the successful value):
//   - a.head.next: CHANGES to b.head.next
//   - a.tail.prev: CHANGES to b.tail.prev
//   - b.head.next: CHANGES to a_first
//   - b.tail.prev: CHANGES to a_last

// === STEP 2: FAILURE PATH ===
// When aws_linked_list_swap_contents returns AWS_OP_ERR (or fails):
//   - a.head.next: UNCHANGED
//   - a.tail.prev: UNCHANGED
//   - b.head.next: UNCHANGED
//   - b.tail.prev: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   a (struct aws_linked_list):
//     - head.next: CHANGED on success, UNCHANGED on failure
//     - head.prev: UNCHANGED always
//     - tail.next: UNCHANGED always
//     - tail.prev: CHANGED on success, UNCHANGED on failure
//   b (struct aws_linked_list):
//     - head.next: CHANGED on success, UNCHANGED on failure
//     - head.prev: UNCHANGED always
//     - tail.next: UNCHANGED always
//     - tail.prev: CHANGED on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&a): YES (must hold after call)
//   - aws_linked_list_is_valid(&b): YES (must hold after call)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    struct aws_linked_list b;
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    aws_linked_list_swap_contents(&a, &b);

    // Assertions for a
    if (!aws_linked_list_empty(&old_b)) {
        assert(a.head.next == old_b.head.next);
        assert(a.tail.prev == old_b.tail.prev);
    } else {
        assert(aws_linked_list_empty(&a));
    }

    // Assertions for b
    if (!aws_linked_list_empty(&old_a)) {
        assert(b.head.next == old_a.head.next);
        assert(b.tail.prev == old_a.tail.prev);
    } else {
        assert(aws_linked_list_empty(&b));
    }

    // Validity invariants
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
}
