// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_insert_after returns AWS_OP_SUCCESS (or the successful value):
//   - after.next: CHANGES to to_add
//   - after.next.prev: CHANGES to to_add
//   - to_add.prev: CHANGES to after
//   - to_add.next: CHANGES to after.next (old value)

// === STEP 2: FAILURE PATH ===
// When aws_linked_list_insert_after returns AWS_OP_ERR (or fails):
//   - after.next: UNCHANGED
//   - after.next.prev: UNCHANGED
//   - to_add.prev: UNCHANGED
//   - to_add.next: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   after (struct aws_linked_list_node):
//     - next: CHANGED on success, UNCHANGED on failure
//     - prev: UNCHANGED always
//   to_add (struct aws_linked_list_node):
//     - next: CHANGED on success, UNCHANGED on failure
//     - prev: CHANGED on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_node_next_is_valid(&after): YES (must hold after call)
//   - aws_linked_list_node_prev_is_valid(&to_add): YES (must hold after call)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list_node after;
    struct aws_linked_list_node to_add;
    struct aws_linked_list_node old_after = after;
    struct aws_linked_list_node old_to_add = to_add;

    ensure_linked_list_is_allocated((struct aws_linked_list *)NULL, 10); // Ensure the list is allocated

    aws_linked_list_insert_after(&after, &to_add);

    // Frame conditions and validity invariants
    assert(after.next == (old_after.next == &to_add ? old_after.next : to_add));
    assert(after.prev == old_after.prev);
    assert(to_add.next == (old_after.next == &to_add ? old_to_add.next : old_after.next));
    assert(to_add.prev == (old_after.next == &to_add ? &after : old_to_add.prev));

    assert(aws_linked_list_node_next_is_valid(&after));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
}
