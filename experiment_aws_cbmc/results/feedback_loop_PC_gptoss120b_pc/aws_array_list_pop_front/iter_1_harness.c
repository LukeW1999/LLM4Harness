#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_pop_front_harness() {
    /* 1. Declare and bound data structure */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_pop_front(&list);

    /* 4. Postconditions for success and failure */
    if (result == AWS_OP_SUCCESS) {
        /* Length decreased by one */
        assert(list.length == old.length - 1);
    } else {
        /* No change on failure */
        assert(list.length == old.length);
        /* Failure must be due to empty list */
        assert(old.length == 0);
        assert(result == AWS_OP_ERR);
    }

    /* 5. Unchanged fields (frame conditions) */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 6. Invariant: list remains valid */
    assert(aws_array_list_is_valid(&list));
}
