#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_pop_front_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_pop_front(&list);

    /* 4. Assert postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: length decreases by 1 */
        assert(list.length == old.length - 1);
        /* Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    } else {
        /* Failure: list unchanged */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        /* Error code must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
    }

    /* 5. Assert validity invariants always hold */
    assert(aws_array_list_is_valid(&list));
}
