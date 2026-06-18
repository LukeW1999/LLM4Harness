#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_get_at_ptr_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* 2. Nondeterministic index, bounded */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);
    /* Prevent overflow in pointer arithmetic */
    __CPROVER_assume(list.item_size == 0 ||
                     index <= SIZE_MAX / list.item_size);

    /* 3. Prepare output pointer */
    void *val_out = NULL;          /* initial value */
    void **val = &val_out;
    void *old_val_out = val_out;   /* saved for failure case */

    /* 4. Call function under test */
    int result = aws_array_list_get_at_ptr(&list, val, index);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success: index must be within length */
        assert(list.length > index);
        /* The returned pointer must point to the correct element */
        assert(*val == (void *)((uint8_t *)list.data +
                                (list.item_size * index)));
    } else {
        /* Failure: list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        /* Output pointer must be unchanged */
        assert(*val == old_val_out);
    }

    /* 6. Fields that never change (regardless of result) */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
