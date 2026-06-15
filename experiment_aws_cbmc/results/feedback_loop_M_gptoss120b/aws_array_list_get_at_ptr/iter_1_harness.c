#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

/* CBMC harness for aws_array_list_get_at_ptr */
void aws_array_list_get_at_ptr_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Nondeterministic index (bounded) */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Output pointer */
    void *val = NULL;
    void **val_ptr = &val;

    /* 5. Call the function under test */
    int result = aws_array_list_get_at_ptr(&list, val_ptr, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success: fields that must remain unchanged */
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);

        /* The returned pointer must point to the correct element */
        void *expected = (void *)((uint8_t *)old.data + (old.item_size * index));
        assert(val == expected);
    } else {
        /* Failure: the whole list must be unchanged */
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);

        /* The output pointer must not be modified */
        assert(val == NULL);
    }

    /* 7. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
