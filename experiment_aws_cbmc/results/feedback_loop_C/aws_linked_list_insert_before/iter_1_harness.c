// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_insert_before returns AWS_OP_SUCCESS (or the successful value):
//   - before->prev: CHANGES to to_add
//   - to_add->next: CHANGES to before
//   - to_add->prev: CHANGES to before->prev
//   - before->prev->next: CHANGES to to_add
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_insert_before returns AWS_OP_ERR (or fails):
//   - before->prev: UNCHANGED
//   - to_add->next: UNCHANGED
//   - to_add->prev: UNCHANGED
//   - before->prev->next: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   before (struct aws_linked_list_node):
//     - next: UNCHANGED always
//     - prev: CHANGED on success, UNCHANGED on failure
//   to_add (struct aws_linked_list_node):
//     - next: CHANGED on success, UNCHANGED on failure
//     - prev: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_node_next_is_valid(before): YES (must hold after call)
//   - aws_linked_list_node_prev_is_valid(before): YES (must hold after call)
//   - aws_linked_list_node_next_is_valid(to_add): YES (must hold after call)
//   - aws_linked_list_node_prev_is_valid(to_add): YES (must hold after call)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list_node before;
    struct aws_linked_list_node to_add;

    // Initialize the nodes with non-deterministic values
    before.next = (struct aws_linked_list_node *)nondet_ptr();
    before.prev = (struct aws_linked_list_node *)nondet_ptr();
    to_add.next = (struct aws_linked_list_node *)nondet_ptr();
    to_add.prev = (struct aws_linked_list_node *)nondet_ptr();

    // Ensure the list structure is valid
    ensure_linked_list_is_allocated((struct aws_linked_list *)nondet_ptr(), nondet_size_t());
    assume(aws_linked_list_node_next_is_valid(&before));
    assume(aws_linked_list_node_prev_is_valid(&before));

    // Save old states
    struct aws_linked_list_node old_before = before;
    struct aws_linked_list_node old_to_add = to_add;

    // Call the function under test
    aws_linked_list_insert_before(&before, &to_add);

    // Assertions for success path
    assert(before.prev == &to_add);
    assert(to_add.next == &before);
    assert(to_add.prev == old_before.prev);
    assert(old_before.prev->next == &to_add);

    // Assertions for frame conditions
    assert(before.next == old_before.next);
    assert(to_add.next == &before || to_add.next == old_to_add.next);
    assert(to_add.prev == old_before.prev || to_add.prev == old_to_add.prev);

    // Assertions for validity invariants
    assert(aws_linked_list_node_next_is_valid(&before));
    assert(aws_linked_list_node_prev_is_valid(&before));
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
}
