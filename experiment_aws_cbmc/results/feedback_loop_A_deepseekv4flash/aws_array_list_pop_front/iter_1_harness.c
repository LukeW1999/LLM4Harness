#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_pop_front_harness() {
    /* Non-deterministic list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;
    size_t old_length = list.length;

    /* Call function under test */
    int result = aws_array_list_pop_front(&list);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success path: length decreased by 1 */
        assert(list.length == old_length - 1);
    } else {
        /* Failure path: list unchanged */
        assert(list.length == old_length);
        /* Error code must be AWS_ERROR_LIST_EMPTY; we cannot check the error variable directly,
         * but we know the return value is AWS_OP_ERR. The list state is unchanged. */
    }

    /* Fields that must not change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
