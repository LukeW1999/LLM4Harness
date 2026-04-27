// === STEP 1: SUCCESS PATH ===
// When aws_array_list_pop_front returns AWS_OP_SUCCESS (or the successful value):
//   - list.current_size: CHANGES to list.current_size - 1
//   - list.length: CHANGES to list.length - 1
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_pop_front returns AWS_OP_ERR (or fails):
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
//     - current_size: CHANGED on success, UNCHANGED on failure
//     - length: CHANGED on success, UNCHANGED on failure
//     - item_size: UNCHANGED always
//     - data: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_pop_front_harness() {
    struct aws_array_list list;
    struct aws_array_list old_list = list;

    // Initialize the list with some values
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    // Save the old state
    old_list = list;

    int result = aws_array_list_pop_front(&list);

    // Check success path
    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size - 1);
        assert(list.length == old_list.length - 1);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    }
    // Check failure path
    else {
        assert(result == aws_raise_error(AWS_ERROR_LIST_EMPTY));
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    }

    // Validity invariant
    assert(aws_array_list_is_valid(&list));
}
