#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_pop_front_harness(void) {
    /* 1. Declare and bound the data structure */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call the function under test */
    int result = aws_array_list_pop_front(&list);

    /* 4. Post‑condition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* On success the length is decremented by one, all other fields stay the same */
        assert(list.length == old.length - 1);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
    } else {
        /* On failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
    }

    /* 5. Fields that must never change regardless of the result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* 6. The list must remain valid after the call */
    assert(aws_array_list_is_valid(&list));
}
