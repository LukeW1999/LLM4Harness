#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_pop_front_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    int result = aws_array_list_pop_front(&list);

    // Postcondition: validity
    assert(aws_array_list_is_valid(&list));

    // Postconditions for success
    assert(!(result == AWS_OP_SUCCESS) || (list.length == old.length - 1));
    assert(!(result == AWS_OP_SUCCESS) || (list.alloc == old.alloc));
    assert(!(result == AWS_OP_SUCCESS) || (list.current_size == old.current_size));
    assert(!(result == AWS_OP_SUCCESS) || (list.item_size == old.item_size));
    assert(!(result == AWS_OP_SUCCESS) || (list.data == old.data));

    // Postconditions for failure
    assert(!(result == AWS_OP_ERR) || (list.length == old.length));
    assert(!(result == AWS_OP_ERR) || (list.alloc == old.alloc));
    assert(!(result == AWS_OP_ERR) || (list.current_size == old.current_size));
    assert(!(result == AWS_OP_ERR) || (list.item_size == old.item_size));
    assert(!(result == AWS_OP_ERR) || (list.data == old.data));

    // Assert result is one of the two expected return codes
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}
