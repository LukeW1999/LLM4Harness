// === STEP 1: SUCCESS PATH ===
// aws_array_list_swap returns void (no return value)
// When a != b:
//   - list->data: the bytes at positions a and b are swapped
//   - list->length: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
// When a == b:
//   - nothing changes (early return)
//
// === STEP 2: FAILURE PATH ===
// The function uses AWS_FATAL_PRECONDITION (aborts on failure), so no error return.
// Preconditions: a < list->length AND b < list->length
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - alloc: UNCHANGED always
//   - current_size: UNCHANGED always
//   - length: UNCHANGED always
//   - item_size: UNCHANGED always
//   - data: CHANGED (bytes at positions a and b swapped) when a != b, UNCHANGED when a == b
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_array_list_swap_harness() {
    // Create a bounded array list
    struct aws_array_list list;
    
    // Bound the list for CBMC tractability
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    
    // Need at least 1 element for valid indices
    __CPROVER_assume(list.length > 0);
    
    // Nondeterministic indices within bounds
    size_t a;
    size_t b;
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);
    
    // Save old state
    struct aws_array_list old_list = list;
    
    // Save old data contents if item_size > 0 and data is valid
    // We'll save copies of the items at positions a and b
    uint8_t *item_a_old = NULL;
    uint8_t *item_b_old = NULL;
    
    if (list.item_size > 0 && list.data != NULL) {
        item_a_old = malloc(list.item_size);
        item_b_old = malloc(list.item_size);
        __CPROVER_assume(item_a_old != NULL);
        __CPROVER_assume(item_b_old != NULL);
        memcpy(item_a_old, (uint8_t *)list.data + a * list.item_size, list.item_size);
        memcpy(item_b_old, (uint8_t *)list.data + b * list.item_size, list.item_size);
    }
    
    // Call the function under test
    aws_array_list_swap(&list, a, b);
    
    // === ASSERTIONS ===
    
    // Frame conditions: structural fields unchanged
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);
    
    // Validity invariant
    assert(aws_array_list_is_valid(&list));
    
    // Data swap assertions
    if (list.item_size > 0 && list.data != NULL && item_a_old != NULL && item_b_old != NULL) {
        uint8_t *data = (uint8_t *)list.data;
        if (a == b) {
            // When a == b, data is unchanged
            assert_bytes_match(data + a * list.item_size, item_a_old, list.item_size);
        } else {
            // When a != b, items at positions a and b are swapped
            assert_bytes_match(data + a * list.item_size, item_b_old, list.item_size);
            assert_bytes_match(data + b * list.item_size, item_a_old, list.item_size);
        }
    }
}
