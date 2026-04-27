// === STEP 1: SUCCESS PATH ===
// When aws_array_list_erase returns AWS_OP_SUCCESS (or the successful value):
//   - list.length: CHANGES to <new value> (specifically, length - 1)
//   - list.data: CHANGES to <new value> (items after index are shifted down)
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_erase returns AWS_OP_ERR (or fails):
//   - list.alloc: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.length: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.data: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_array_list):
//     - alloc: UNCHANGED always
//     - current_size: UNCHANGED always
//     - length: CHANGED on success, UNCHANGED on failure
//     - item_size: UNCHANGED always
//     - data: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_erase_harness() {
    struct aws_array_list list;
    size_t index;

    // Initialize list with arbitrary values
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_ITEM_ALLOC, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    // Save old state of the list
    struct aws_array_list old_list = list;

    // Ensure index is within bounds for the length of the list
    __CPROVER_assume(index < list.length);

    int result = aws_array_list_erase(&list, index);

    if (result == AWS_OP_SUCCESS) {
        // Check frame conditions for success path
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length - 1);
        assert(list.item_size == old_list.item_size);
        // Data should have changed due to removal and shifting
    } else {
        // Check frame conditions for failure path
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    }

    // Check validity invariant
    assert(aws_array_list_is_valid(&list));
}
