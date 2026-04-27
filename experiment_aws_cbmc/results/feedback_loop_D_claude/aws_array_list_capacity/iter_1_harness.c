// === STEP 1: SUCCESS PATH ===
// aws_array_list_capacity is not an int-returning function; it returns a size_t.
// It computes: capacity = list->current_size / list->item_size
// Preconditions: list->item_size != 0 (FATAL), aws_array_list_is_valid(list)
// Return value: list->current_size / list->item_size
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always returns a size_t value (no error code).
// The function has FATAL preconditions that abort if violated.
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - alloc: UNCHANGED always
//   - data: UNCHANGED always
//   - current_size: UNCHANGED always
//   - length: UNCHANGED always
//   - item_size: UNCHANGED always
// The function is a pure read - no fields are modified.
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(list): YES (postcondition asserted in implementation)
//   - Return value equals list->current_size / list->item_size

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_capacity_harness(void) {
    /* Allocate and initialize the array list */
    struct aws_array_list list;

    /* Use bounded list to keep verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume valid preconditions */
    __CPROVER_assume(aws_array_list_is_valid(&list));
    /* FATAL precondition: item_size must be non-zero */
    __CPROVER_assume(list.item_size != 0);

    /* Save old state */
    struct aws_array_list old = list;

    /* Call the function */
    size_t capacity = aws_array_list_capacity(&list);

    /* === STEP 1: Verify return value === */
    assert(capacity == list.current_size / list.item_size);

    /* === STEP 3: Frame conditions - no fields should change === */
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);

    /* === STEP 4: Validity invariants === */
    assert(aws_array_list_is_valid(&list));
}
