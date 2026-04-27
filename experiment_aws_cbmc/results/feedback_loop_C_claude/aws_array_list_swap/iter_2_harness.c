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
}
