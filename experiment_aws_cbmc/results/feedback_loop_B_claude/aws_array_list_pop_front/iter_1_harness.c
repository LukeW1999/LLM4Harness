#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_array_list_pop_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_pop_front(&list);

    /* 4. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: list was non-empty, length decreased by 1 */
        assert(old.length > 0);
        assert(list.length == old.length - 1);
        /* item_size, alloc, current_size, data pointer do not change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    } else {
        /* Failure: list was empty, nothing changes */
        assert(old.length == 0);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 5. Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
