// === STEP 1: SUCCESS PATH ===
// When aws_array_list_swap returns AWS_OP_SUCCESS (or the successful value):
//   - list.data[a]: CHANGES to the value at list.data[b] before the swap
//   - list.data[b]: CHANGES to the value at list.data[a] before the swap
//   - list.current_size: UNCHANGED
//   - list.length: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.alloc: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_swap returns AWS_OP_ERR (or fails):
//   - list.data[a]: UNCHANGED
//   - list.data[b]: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.length: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.alloc: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct type):
//     - data: CHANGED on success, UNCHANGED on failure
//     - current_size: UNCHANGED always
//     - length: UNCHANGED always
//     - item_size: UNCHANGED always
//     - alloc: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/cbmc_utils.h>

void aws_array_list_swap_harness() {
    struct aws_array_list list;
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    // Ensure the list is properly allocated and initialized
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(a < list.length && b < list.length);

    // Save old state
    struct aws_array_list old_list = list;

    // Call the function under test
    aws_array_list_swap(&list, a, b);

    // Assertions for success path
    if (a != b) {
        // Check that the elements at indices a and b have been swapped
        assert(memcmp((char *)list.data + a * list.item_size, (char *)old_list.data + b * old_list.item_size, list.item_size) == 0);
        assert(memcmp((char *)list.data + b * list.item_size, (char *)old_list.data + a * old_list.item_size, list.item_size) == 0);
    }

    // Assertions for frame conditions
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    // Assertions for validity invariants
    assert(aws_array_list_is_valid(&list));
}
