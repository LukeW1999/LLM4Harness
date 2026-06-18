#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_erase_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save a copy of the original state */
    struct aws_array_list old = list;

    /* 2. Non‑deterministic index */
    size_t index = nondet_size_t();

    /* 3. Call the function under test */
    int result = aws_array_list_erase(&list, index);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success implies the index was within bounds and length decreased by one */
        assert(old.length > 0);
        assert(index < old.length);
        assert(list.length == old.length - 1);
    } else {
        /* On failure the list must be unchanged */
        assert(list.length == old.length);
    }

    /* 5. Fields that must remain unchanged regardless of success/failure */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* 6. Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
