// === STEP 1: SUCCESS PATH ===
// When aws_array_list_clear returns AWS_OP_SUCCESS (or the successful value):
//   - list.length: CHANGES to 0
//   - list.data: UNCHANGED (content cleared but pointer remains the same)
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_clear returns AWS_OP_ERR (or fails):
//   - list.length: UNCHANGED
//   - list.data: UNCHANGED
//   - list.alloc: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.item_size: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_array_list):
//     - length: CHANGED on success, UNCHANGED on failure
//     - data: UNCHANGED always
//     - alloc: UNCHANGED always
//     - current_size: UNCHANGED always
//     - item_size: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clear_harness() {
    struct aws_array_list list;
    struct aws_array_list old_list = list;
    ensure_array_list_has_allocated_data_member(&list);

    // Assume preconditions
    __CPROVER_assume(aws_array_list_is_valid(&list));

    int result = aws_array_list_clear(&list);

    // Check postconditions
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == 0);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
    } else {
        assert(list.length == old_list.length);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
    }

    // Validity invariant
    assert(aws_array_list_is_valid(&list));
}
