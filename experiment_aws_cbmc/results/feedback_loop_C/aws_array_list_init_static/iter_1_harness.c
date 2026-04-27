// === STEP 1: SUCCESS PATH ===
// When aws_array_list_init_static returns AWS_OP_SUCCESS (or the successful value):
//   - list->alloc: CHANGES to NULL
//   - list->current_size: CHANGES to item_count * item_size
//   - list->item_size: CHANGES to item_size
//   - list->length: CHANGES to 0
//   - list->data: CHANGES to raw_array
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_init_static returns AWS_OP_ERR (or fails):
//   - list->alloc: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->length: UNCHANGED
//   - list->data: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_array_list):
//     - alloc: CHANGED on success, UNCHANGED on failure
//     - current_size: CHANGED on success, UNCHANGED on failure
//     - item_size: CHANGED on success, UNCHANGED on failure
//     - length: CHANGED on success, UNCHANGED on failure
//     - data: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_init_static_harness() {
    struct aws_array_list list;
    void *raw_array = (void *)nondet_size_t();
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();

    // Save old state
    struct aws_array_list old_list = list;

    // Assumptions for the input parameters
    __CPROVER_assume(raw_array != NULL);
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &(list.current_size)));

    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    // Assertions for the success path
    assert(list.alloc == NULL);
    assert(list.current_size == item_count * item_size);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);

    // Assertions for the frame conditions
    assert(list.alloc == NULL); // Changed on success
    assert(list.current_size == item_count * item_size); // Changed on success
    assert(list.item_size == item_size); // Changed on success
    assert(list.length == 0); // Changed on success
    assert(list.data == raw_array); // Changed on success

    // Validity invariant
    assert(aws_array_list_is_valid(&list));
}
