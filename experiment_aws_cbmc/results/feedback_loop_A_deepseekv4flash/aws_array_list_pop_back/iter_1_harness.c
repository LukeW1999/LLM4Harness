#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_erase_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t index;

    /* Bound the list to limit state space */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* Call the function */
    int result = aws_array_list_erase(&list, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, length must decrease by 1 */
        assert(list.length == old.length - 1);
        /* Data pointer and current_size remain unchanged */
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    } else {
        /* On failure (index out of bounds), list must be unchanged */
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    }

    /* Unchanged fields for both success and failure */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* Validity invariant must hold after call */
    assert(aws_array_list_is_valid(&list));
}
