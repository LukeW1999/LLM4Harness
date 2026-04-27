// === STEP 1: SUCCESS PATH ===
// When aws_array_list_length returns AWS_OP_SUCCESS (or the successful value):
//   - param1.length: RETURNS the current length of the list
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_length returns AWS_OP_ERR (or fails):
//   - param1.length: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   param1 (struct aws_array_list):
//     - alloc: UNCHANGED always
//     - data: UNCHANGED always
//     - item_size: UNCHANGED always
//     - length: CHANGED on success, UNCHANGED on failure
//     - current_size: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&param1): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_length_harness() {
    struct aws_array_list list;
    ensure_array_list_has_allocated_data_member(&list);

    // Capture the old state of the list
    struct aws_array_list old_list = list;

    // Call the function under test
    size_t result = aws_array_list_length(&list);

    // Assertions based on the analysis
    assert(aws_array_list_is_valid(&list));

    if (result != AWS_OP_ERR) {
        // Success path assertions
        assert(list.length == result);
    } else {
        // Failure path assertions
        assert(list.alloc == old_list.alloc);
        assert(list.data == old_list.data);
        assert(list.item_size == old_list.item_size);
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
    }
}
