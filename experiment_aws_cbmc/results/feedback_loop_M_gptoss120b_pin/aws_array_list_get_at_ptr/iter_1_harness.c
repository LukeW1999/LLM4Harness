#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_get_at_ptr_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non‑deterministic index, bounded */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Output pointer */
    void *val = NULL;
    void *old_val = val;

    /* 4. Save old state of the list */
    struct aws_array_list old = list;

    /* 5. Call the function under test */
    int result = aws_array_list_get_at_ptr(&list, &val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* val must point to the element at the given index */
        assert(val == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        /* On failure, val must be unchanged */
        assert(val == old_val);
    }

    /* 7. List fields must remain unchanged regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 8. Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
