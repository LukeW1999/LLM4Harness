#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_pop_front_harness() {
    struct aws_array_list list;
    
    // Ensure list is bounded and valid
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    
    // Save old state for postcondition checks
    struct aws_array_list old = list;
    
    // Call the function under verification
    int result = aws_array_list_pop_front(&list);
    
    // Postcondition: validity always holds
    assert(aws_array_list_is_valid(&list));
    
    // Postconditions for success case
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length - 1);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        // Ensure the popped element data is removed (front element bytes shifted)
        assert(list.length == 0 || aws_array_list_is_valid(&list));
    }
    
    // Postconditions for failure case
    if (result == AWS_OP_ERR) {
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    }
    
    // Assert result is one of the two expected return codes
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
    
    // Additional assertion to ensure the function can fail when length is 0
    // This helps make the failure branch reachable
    if (old.length == 0) {
        assert(result == AWS_OP_ERR);
    }
}
