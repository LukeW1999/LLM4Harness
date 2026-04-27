// === STEP 1: SUCCESS PATH ===
// When aws_array_list_clean_up returns AWS_OP_SUCCESS (or the successful value):
//   - list.alloc: SET to NULL
//   - list.data: SET to NULL
//   - list.current_size: SET to 0
//   - list.length: SET to 0
//   - list.item_size: SET to 0
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_clean_up returns AWS_OP_ERR (or fails):
//   - list.alloc: UNCHANGED
//   - list.data: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.length: UNCHANGED
//   - list.item_size: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_array_list):
//     - alloc: CHANGED on success, UNCHANGED on failure
//     - current_size: CHANGED on success, UNCHANGED on failure
//     - length: CHANGED on success, UNCHANGED on failure
//     - item_size: CHANGED on success, UNCHANGED on failure
//     - data: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_clean_up_harness() {
    struct aws_array_list list;
    struct aws_array_list old_list = list;

    // Initialize list with some values
    list.alloc = (struct aws_allocator *)nondet_ptr();
    list.current_size = nondet_size_t();
    list.length = nondet_size_t();
    list.item_size = nondet_size_t();
    list.data = (void *)nondet_ptr();

    // Call the function under test
    aws_array_list_clean_up(&list);

    // Success path assertions
    if (old_list.alloc && old_list.data) {
        assert(list.alloc == NULL);
        assert(list.data == NULL);
        assert(list.current_size == 0);
        assert(list.length == 0);
        assert(list.item_size == 0);
    } else {
        // Failure path assertions
        assert(list.alloc == old_list.alloc);
        assert(list.data == old_list.data);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
    }

    // Validity invariant
    assert(aws_array_list_is_valid(&list));
}
