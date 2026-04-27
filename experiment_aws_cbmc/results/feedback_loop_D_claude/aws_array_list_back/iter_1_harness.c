// === STEP 1: SUCCESS PATH ===
// When aws_array_list_back returns AWS_OP_SUCCESS:
//   - list->alloc: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->length: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->data: UNCHANGED (pointer itself unchanged, contents unchanged)
//   - val: CHANGED - contains a copy of the last element (item_size bytes copied from list->data)
//   - return value: AWS_OP_SUCCESS (0)
//   - condition: list->length > 0
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_back returns AWS_OP_ERR (list is empty):
//   - list->alloc: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->length: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->data: UNCHANGED
//   - val: UNCHANGED (memcpy not called)
//   - return value: AWS_OP_ERR (-1) via aws_raise_error
//   - condition: list->length == 0
//
// === STEP 3: FRAME CONDITIONS ===
//   list (struct aws_array_list):
//     - alloc: UNCHANGED always
//     - current_size: UNCHANGED always
//     - length: UNCHANGED always
//     - item_size: UNCHANGED always
//     - data: UNCHANGED always (pointer)
//   val (void*):
//     - contents: CHANGED on success (last item copied in), UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(list): YES (postcondition asserted in both paths)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

void aws_array_list_back_harness(void) {
    /* Allocate and initialize the list with bounded sizes */
    struct aws_array_list list;
    
    /* Ensure bounded for CBMC tractability */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    
    /* Assume the list is valid as a precondition */
    __CPROVER_assume(aws_array_list_is_valid(&list));
    
    /* Save old state */
    struct aws_array_list old_list = list;
    
    /* Allocate val buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    
    /* Save old val contents */
    uint8_t *old_val = malloc(list.item_size);
    __CPROVER_assume(old_val != NULL);
    memcpy(old_val, val, list.item_size);
    
    /* Call the function under test */
    int result = aws_array_list_back(&list, val);
    
    /* === FRAME CONDITIONS: list fields are always unchanged === */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);
    
    /* === VALIDITY INVARIANT === */
    assert(aws_array_list_is_valid(&list));
    
    if (result == AWS_OP_SUCCESS) {
        /* === SUCCESS PATH === */
        /* list was non-empty */
        assert(old_list.length > 0);
        
        /* val now contains the last element */
        size_t last_item_offset = old_list.item_size * (old_list.length - 1);
        assert_bytes_match(
            (const uint8_t *)val,
            (const uint8_t *)old_list.data + last_item_offset,
            old_list.item_size
        );
    } else {
        /* === FAILURE PATH === */
        assert(result == AWS_OP_ERR);
        
        /* list was empty */
        assert(old_list.length == 0);
        
        /* val is unchanged */
        assert_bytes_match((const uint8_t *)val, old_val, list.item_size);
    }
}
