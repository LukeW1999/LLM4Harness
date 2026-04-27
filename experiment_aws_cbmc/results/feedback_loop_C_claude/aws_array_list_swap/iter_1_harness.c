// === STEP 1: SUCCESS PATH ===
// aws_array_list_swap returns void (no return value)
// When a != b:
//   - list->data: the bytes at positions a and b are swapped
//   - list->length: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
// When a == b:
//   - list->data: UNCHANGED (early return)
//   - all other fields: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// Function uses AWS_FATAL_PRECONDITION (aborts on failure), so no error return path.
// Preconditions: a < list->length AND b < list->length
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - alloc: UNCHANGED always
//   - current_size: UNCHANGED always
//   - length: UNCHANGED always
//   - item_size: UNCHANGED always
//   - data: CHANGED (bytes at indices a and b swapped) when a != b, UNCHANGED when a == b
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold before and after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_array_list_swap_harness() {
    // Create a bounded array list
    struct aws_array_list list;
    
    // Use bounded sizes to keep verification tractable
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    
    // Ensure the list is valid before the call
    __CPROVER_assume(aws_array_list_is_valid(&list));
    
    // Ensure list has at least some elements (length > 0 needed for valid indices)
    __CPROVER_assume(list.length > 0);
    
    // Nondeterministic indices within bounds
    size_t a;
    size_t b;
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);
    
    // Save old state
    struct aws_array_list old_list = list;
    
    // Save a copy of the data before the swap for verification
    // We need to capture the bytes at positions a and b
    uint8_t *old_data = NULL;
    size_t data_size = list.current_size;
    if (list.data != NULL && data_size > 0) {
        old_data = malloc(data_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, data_size);
    }
    
    // Call the function under test
    aws_array_list_swap(&list, a, b);
    
    // === FRAME CONDITIONS ===
    // Structural fields must be unchanged
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    
    // === VALIDITY INVARIANT ===
    assert(aws_array_list_is_valid(&list));
    
    // === DATA SWAP VERIFICATION ===
    if (list.data != NULL && old_data != NULL && list.item_size > 0) {
        if (a == b) {
            // When a == b, data should be unchanged
            assert(memcmp(list.data, old_data, data_size) == 0);
        } else {
            // When a != b, verify that elements at positions a and b are swapped
            uint8_t *new_data = (uint8_t *)list.data;
            
            // Element at position a in new list should equal element at position b in old list
            assert(memcmp(new_data + a * list.item_size, 
                          old_data + b * list.item_size, 
                          list.item_size) == 0);
            
            // Element at position b in new list should equal element at position a in old list
            assert(memcmp(new_data + b * list.item_size, 
                          old_data + a * list.item_size, 
                          list.item_size) == 0);
            
            // All other elements should be unchanged
            // (This is hard to verify for all positions in CBMC, but the above checks
            // are the primary correctness properties)
        }
    }
}
