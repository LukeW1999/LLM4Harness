// === STEP 1: SUCCESS PATH ===
// When aws_array_list_erase returns AWS_OP_SUCCESS (or the successful value):
//   - list.length: CHANGES to length - 1
//   - list.data: CHANGES to reflect the removed element and shifted elements
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
//     - current_size: UNCHANGED on success, UNCHANGED on failure
//     - length: CHANGED on success, UNCHANGED on failure
//     - item_size: UNCHANGED always
//     - data: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/cbmc_utils.h>

void aws_array_list_erase_harness() {
    struct aws_array_list list;
    size_t index;

    // Initialize the list with some arbitrary values
    size_t max_item_alloc = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    assume(max_item_alloc > 0 && max_item_size > 0);
    assume(max_item_alloc <= MAX_INITIAL_ITEM_ALLOCATION);
    assume(max_item_size <= MAX_ITEM_SIZE);

    // Ensure the list is properly allocated and initialized
    ensure_array_list_has_allocated_data_member(&list);
    list.alloc = (struct aws_allocator *)any_ptr();
    list.current_size = max_item_alloc * max_item_size;
    list.length = nondet_size_t();
    assume(list.length <= max_item_alloc);
    list.item_size = max_item_size;
    list.data = bounded_malloc(list.current_size);

    // Save old state
    struct aws_array_list old_list = list;
    size_t old_length = list.length;

    // Non-deterministically choose an index
    index = nondet_size_t();
    assume(index < list.length);

    // Call the function under test
    int result = aws_array_list_erase(&list, index);

    // Success path assertions
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_length - 1);
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.data != NULL);  // Data pointer should still be valid
    }
    // Failure path assertions
    else {
        assert(list.length == old_length);
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    }

    // Validity invariant
    assert(aws_array_list_is_valid(&list));
}
