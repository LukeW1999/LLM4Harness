// === STEP 1: SUCCESS PATH ===
// When aws_array_list_push_front returns AWS_OP_SUCCESS (0):
//   - list->length: CHANGES to orig_len + 1
//   - list->data: first item_size bytes contain a copy of val
//   - list->current_size: MAY CHANGE (if capacity was expanded)
//   - list->alloc: UNCHANGED
//   - list->item_size: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_push_front returns AWS_OP_ERR (-1):
//   - list->length: UNCHANGED
//   - list->data: UNCHANGED (no modification)
//   - list->current_size: UNCHANGED
//   - list->alloc: UNCHANGED
//   - list->item_size: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
//   list (struct aws_array_list):
//     - alloc: UNCHANGED always
//     - item_size: UNCHANGED always
//     - length: CHANGED on success (orig_len + 1), UNCHANGED on failure
//     - current_size: CHANGED on success (may grow), UNCHANGED on failure
//     - data: CHANGED on success (elements shifted, val copied to front), UNCHANGED on failure
//   val (const void *):
//     - pointed-to memory: UNCHANGED always (only read)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call on success)
//   - val must point to readable memory of item_size bytes

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_array_list_push_front_harness() {
    // Declare and set up the array list
    struct aws_array_list list;
    
    // Bound the list to keep verification tractable
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    
    // Ensure item_size is non-zero (required for valid list)
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    
    // Ensure allocated data member
    ensure_array_list_has_allocated_data_member(&list);
    
    // Assume the list is valid before the call
    __CPROVER_assume(aws_array_list_is_valid(&list));
    
    // Allocate val with item_size bytes of readable memory
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    
    // Save old state
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;
    
    // Save a copy of val's content for later comparison
    uint8_t *val_copy = malloc(list.item_size);
    __CPROVER_assume(val_copy != NULL);
    memcpy(val_copy, val, list.item_size);
    
    // Call the function under test
    int result = aws_array_list_push_front(&list, val);
    
    // === Verify frame conditions that are ALWAYS true ===
    // alloc and item_size never change
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);
    
    if (result == AWS_OP_SUCCESS) {
        // === SUCCESS PATH assertions ===
        
        // Length must have increased by 1
        assert(list.length == old_length + 1);
        
        // The list must still be valid
        assert(aws_array_list_is_valid(&list));
        
        // The first element must be a copy of val
        assert(list.data != NULL);
        assert_bytes_match((const uint8_t *)list.data, val_copy, list.item_size);
        
        // current_size must be at least enough to hold the new length
        assert(list.current_size >= list.length * list.item_size);
        
    } else {
        // === FAILURE PATH assertions ===
        
        // Length must be unchanged
        assert(list.length == old_length);
        
        // current_size must be unchanged
        assert(list.current_size == old_current_size);
    }
}
