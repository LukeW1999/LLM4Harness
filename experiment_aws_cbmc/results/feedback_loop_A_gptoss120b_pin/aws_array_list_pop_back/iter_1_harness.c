#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_erase_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Choose a nondeterministic index */
    size_t idx = nondet_size_t();

    /* 4. Call the function under test */
    int result = aws_array_list_erase(&list, idx);

    /* 5. Global invariant must hold */
    assert(aws_array_list_is_valid(&list));

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success implies the index was within bounds and length decreased by one */
        assert(old.length > 0);
        assert(idx < old.length);
        assert(list.length == old.length - 1);

        /* Fields that must not change */
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    } else {
        /* Failure: the list must be unchanged */
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    }
}
