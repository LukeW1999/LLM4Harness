// === STEP 1: SUCCESS PATH ===
// When aws_array_list_clear returns AWS_OP_SUCCESS (or the successful value):
//   - list.length: CHANGES to 0
//   - list.data: UNCHANGED (but may be zeroed in debug build)
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_clear returns AWS_OP_ERR (or fails):
//   - list.length: UNCHANGED
//   - list.data: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.alloc: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_array_list):
//     - length: CHANGED on success, UNCHANGED on failure
//     - data: UNCHANGED on success, UNCHANGED on failure
//     - current_size: UNCHANGED always
//     - item_size: UNCHANGED always
//     - alloc: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>

void aws_array_list_clear_harness() {
    struct aws_array_list list;
    struct aws_array_list old_list = list;
    size_t max_item_alloc = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(&list, max_item_alloc, max_item_size));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    aws_array_list_clear(&list);

    // Frame conditions
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    // Validity invariant
    assert(aws_array_list_is_valid(&list));

    // Success path
    if (old_list.data != NULL) {
        assert(list.length == 0);
    }

    // Failure path (no failure path for aws_array_list_clear)
}
