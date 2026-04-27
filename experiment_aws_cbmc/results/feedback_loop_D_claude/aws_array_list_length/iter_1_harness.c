// === STEP 1: SUCCESS PATH ===
// aws_array_list_length is a size_t return function (not int), it always returns list->length
// It does NOT modify the list at all - it's a pure read function
// Return value: list->length (the current length of the list)
//
// === STEP 2: FAILURE PATH ===
// There is no failure path - aws_array_list_length always returns list->length
// The function has no error return; it returns size_t
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - alloc: UNCHANGED always
//   - data: UNCHANGED always
//   - current_size: UNCHANGED always
//   - length: UNCHANGED always
//   - item_size: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
// Precondition: AWS_IS_ZEROED(*list) || aws_array_list_is_valid(list)
// Postcondition: AWS_IS_ZEROED(*list) || aws_array_list_is_valid(list)
// Return value equals list->length (the length field)
// The function asserts !list->length || list->data (non-empty list must have data)

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_array_list_length_harness(void) {
    // Allocate and set up the array list
    struct aws_array_list list;

    // Use bounded list to keep verification tractable
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    // Precondition: list must be valid (or zeroed)
    __CPROVER_assume(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));

    // Save old state before calling
    struct aws_array_list old_list = list;

    // Call the function under test
    size_t result = aws_array_list_length(&list);

    // === STEP 1 assertions: return value ===
    // The function returns list->length
    assert(result == old_list.length);

    // === STEP 3 assertions: frame conditions (nothing changes) ===
    assert(list.alloc == old_list.alloc);
    assert(list.data == old_list.data);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);

    // === STEP 4 assertions: validity invariants ===
    // Postcondition: list is still valid (or zeroed)
    assert(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));

    // The function's internal assertion: non-empty list must have data
    // (This is encoded as a precondition in the function itself via AWS_FATAL_PRECONDITION)
    // If length > 0, data must not be null
    if (list.length > 0) {
        assert(list.data != NULL);
    }
}
