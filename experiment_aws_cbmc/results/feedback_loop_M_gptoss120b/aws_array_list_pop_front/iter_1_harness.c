#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* CBMC harness for aws_array_list_pop_front */
void aws_array_list_pop_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Call the function under test */
    int result = aws_array_list_pop_front(&list);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success: length decreased by one */
        assert(list.length == old.length - 1);

        /* Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    } else {
        /* Failure: list unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(result != AWS_OP_SUCCESS);
    }

    /* 5. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
