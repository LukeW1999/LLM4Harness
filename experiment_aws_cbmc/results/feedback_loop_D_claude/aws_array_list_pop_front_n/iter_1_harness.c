// === STEP 1: SUCCESS PATH ===
// The function is void, no return value.
// Case 1: n == 0 → nothing changes (early return from n > 0 check)
// Case 2: 0 < n < list->length:
//   - list->length: CHANGES to (old_length - n)
//   - list->data: contents shifted (memmove), but pointer unchanged
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
// Case 3: n >= list->length:
//   - aws_array_list_clear(list) is called → list->length = 0
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//   - list->data: UNCHANGED (pointer)
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void and always succeeds.
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - alloc: UNCHANGED always
//   - current_size: UNCHANGED always
//   - item_size: UNCHANGED always
//   - data (pointer): UNCHANGED always (memmove modifies contents, not pointer)
//   - length: CHANGED
//     - if n == 0: UNCHANGED (same value)
//     - if n >= old_length: becomes 0
//     - if 0 < n < old_length: becomes old_length - n
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)
//   - The list must be valid before the call too

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_array_list_pop_front_n_harness() {
    // Create a bounded array list
    struct aws_array_list list;
    
    // Use bounded sizes to keep verification tractable
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    
    // Save old state
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data_ptr = list.data;
    
    // Nondeterministic n
    size_t n;
    n = nondet_size_t();
    
    // Call the function
    aws_array_list_pop_front_n(&list, n);
    
    // === FRAME CONDITIONS ===
    // alloc never changes
    assert(list.alloc == old_alloc);
    // current_size never changes
    assert(list.current_size == old_current_size);
    // item_size never changes
    assert(list.item_size == old_item_size);
    // data pointer never changes
    assert(list.data == old_data_ptr);
    
    // === LENGTH POST-CONDITIONS ===
    if (n == 0) {
        // Nothing changes
        assert(list.length == old_length);
    } else if (n >= old_length) {
        // aws_array_list_clear called → length = 0
        assert(list.length == 0);
    } else {
        // 0 < n < old_length
        assert(list.length == old_length - n);
    }
    
    // === VALIDITY INVARIANT ===
    assert(aws_array_list_is_valid(&list));
}
