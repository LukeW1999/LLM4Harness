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

    /* 3. Nondeterministic index */
    size_t idx = nondet_size_t();

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, idx);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length must decrease by one */
        assert(old.length > 0);
        assert(list.length == old.length - 1);
    } else {
        /* on failure the list must be unchanged */
        assert(list.length == old.length);
    }

    /* 6. Fields that must not change */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
