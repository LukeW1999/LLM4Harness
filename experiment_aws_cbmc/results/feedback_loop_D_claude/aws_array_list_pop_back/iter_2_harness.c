#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness(void) {
    /* Allocate and initialize the array list */
    struct aws_array_list list;

    /* Use bounded list to keep verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the list has allocated data */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;

    /* Call the function under test */
    int result = aws_array_list_pop_back(&list);

    /* === STEP 4: Validity invariant === */
    assert(aws_array_list_is_valid(&list));

    /* === STEP 3: Frame conditions - always unchanged === */
    assert(list.data == old_list.data);
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* === STEP 1: Success path === */
        /* Precondition for success: old_length > 0 */
        assert(old_length > 0);

        /* length decremented by 1 */
        assert(list.length == old_length - 1);

        /* Return value is AWS_OP_SUCCESS */
        assert(result == 0);
    } else {
        /* === STEP 2: Failure path === */
        assert(result == AWS_OP_ERR);

        /* Precondition for failure: old_length == 0 */
        assert(old_length == 0);

        /* length unchanged */
        assert(list.length == old_list.length);

        /* Error should be AWS_ERROR_LIST_EMPTY */
        assert(aws_last_error() == AWS_ERROR_LIST_EMPTY);
    }
}
