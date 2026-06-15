#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

/* Harness for aws_array_list_erase */
void aws_array_list_erase_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* 2. Non‑deterministic index */
    size_t index = nondet_size_t();

    /* 3. Call the function under test */
    int result = aws_array_list_erase(&list, index);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success: element removed, length decreased by 1 */
        assert(list.length == old.length - 1);

        /* Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    } else {
        /* Failure: index out of bounds, list unchanged */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 5. Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
