// === STEP 1: SUCCESS PATH ===
// aws_array_list_capacity is a pure query function that returns a size_t value.
// It does NOT return AWS_OP_SUCCESS/AWS_OP_ERR - it returns the capacity (current_size / item_size).
// The function does not modify the list at all.
//   - list->data: UNCHANGED
//   - list->length: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
// Return value: list->current_size / list->item_size
//
// === STEP 2: FAILURE PATH ===
// There is no failure path - the function always returns a size_t.
// It has a FATAL_PRECONDITION that item_size != 0, so we must ensure item_size > 0.
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - data: UNCHANGED always
//   - length: UNCHANGED always
//   - current_size: UNCHANGED always
//   - item_size: UNCHANGED always
//   - alloc: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold before and after call)
//   - Return value equals list->current_size / list->item_size

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_capacity_harness(void) {
    // Allocate and initialize a nondeterministic array list
    struct aws_array_list list;
    
    // Bound the list to keep verification tractable
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    
    // Ensure the list has allocated data
    ensure_array_list_has_allocated_data_member(&list);
    
    // Precondition: list must be valid
    __CPROVER_assume(aws_array_list_is_valid(&list));
    
    // Precondition: item_size must be non-zero (FATAL_PRECONDITION in function)
    __CPROVER_assume(list.item_size > 0);
    
    // Save old state before calling
    struct aws_array_list old_list = list;
    
    // Call the function under verification
    size_t capacity = aws_array_list_capacity(&list);
    
    // === STEP 1 assertions: return value correctness ===
    assert(capacity == list.current_size / list.item_size);
    
    // === STEP 3 assertions: frame conditions - nothing should change ===
    assert(list.data == old_list.data);
    assert(list.length == old_list.length);
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);
    
    // === STEP 4 assertions: validity invariants ===
    assert(aws_array_list_is_valid(&list));
}
