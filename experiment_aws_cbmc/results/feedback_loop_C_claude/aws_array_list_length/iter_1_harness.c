// === STEP 1: SUCCESS PATH ===
// aws_array_list_length is a simple getter that returns list->length
// It does not return AWS_OP_SUCCESS/ERR - it returns a size_t value (the length)
// The function does NOT modify any fields of the list struct
//   - list->length: UNCHANGED
//   - list->data: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
// Return value: equals list->length
//
// === STEP 2: FAILURE PATH ===
// aws_array_list_length has no failure path - it always returns list->length
// All fields remain unchanged regardless
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - length: UNCHANGED always
//   - data: UNCHANGED always
//   - current_size: UNCHANGED always
//   - item_size: UNCHANGED always
//   - alloc: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold before and after call)
//   - Return value equals list->length

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_array_list_length_harness() {
    // Allocate and initialize the array list with nondeterministic but valid state
    struct aws_array_list list;
    
    // Use bounded list to keep verification tractable
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    
    // Save old state before calling
    struct aws_array_list old_list = list;
    
    // Call the function under test
    size_t result = aws_array_list_length(&list);
    
    // === STEP 1 assertions: return value equals list->length ===
    assert(result == old_list.length);
    
    // === STEP 3 assertions: frame conditions - nothing changes ===
    assert(list.length == old_list.length);
    assert(list.data == old_list.data);
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);
    
    // === STEP 4 assertions: validity invariants ===
    assert(aws_array_list_is_valid(&list));
}
