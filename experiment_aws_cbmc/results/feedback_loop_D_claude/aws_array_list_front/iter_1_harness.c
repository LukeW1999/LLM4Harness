// === STEP 1: SUCCESS PATH ===
// When aws_array_list_front returns AWS_OP_SUCCESS:
//   - list->data: UNCHANGED (we only read from it)
//   - list->length: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//   - val: CHANGES to contain a copy of the first item_size bytes of list->data
//   Condition: aws_array_list_length(list) > 0
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_front returns AWS_OP_ERR:
//   - list->data: UNCHANGED
//   - list->length: UNCHANGED (remains 0)
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//   - val: UNCHANGED (memcpy not called)
//   Condition: aws_array_list_length(list) == 0
//
// === STEP 3: FRAME CONDITIONS ===
//   list (struct aws_array_list):
//     - data: UNCHANGED always (function is const on list)
//     - length: UNCHANGED always
//     - current_size: UNCHANGED always
//     - item_size: UNCHANGED always
//     - alloc: UNCHANGED always
//   val (void*):
//     - contents: CHANGED on success (memcpy'd from list->data), UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(list): YES (must hold after call, postcondition)

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

void aws_array_list_front_harness(void) {
    // Create a valid array list
    struct aws_array_list list;
    
    // Bound the list for CBMC tractability
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    
    // Save old state
    struct aws_array_list old_list = list;
    
    // Create val buffer of item_size bytes
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    
    // Save old val contents
    uint8_t *old_val = malloc(list.item_size);
    __CPROVER_assume(old_val != NULL);
    memcpy(old_val, val, list.item_size);
    
    // Call the function
    int result = aws_array_list_front(&list, val);
    
    // === STEP 4: Validity invariant ===
    assert(aws_array_list_is_valid(&list));
    
    // === STEP 3: Frame conditions - list fields always unchanged ===
    assert(list.data == old_list.data);
    assert(list.length == old_list.length);
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);
    
    if (result == AWS_OP_SUCCESS) {
        // === STEP 1: Success path ===
        // list->length > 0
        assert(old_list.length > 0);
        // val now contains the first item_size bytes of list->data
        assert(memcmp(val, list.data, list.item_size) == 0);
    } else {
        // === STEP 2: Failure path ===
        // list->length == 0
        assert(old_list.length == 0);
        // val is unchanged
        assert(memcmp(val, old_val, list.item_size) == 0);
    }
}
