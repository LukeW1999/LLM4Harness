#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_get_at_ptr_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                              MAX_INITIAL_ITEM_ALLOCATION,
                                              MAX_ITEM_SIZE));
    list.alloc = aws_default_allocator();
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);               /* avoid zero‑size multiplication */

    /* 2. Non‑deterministic index, bounded to avoid overflow */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= SIZE_MAX / list.item_size);

    /* 3. Output pointer (must be non‑NULL) */
    void *out_ptr = (void *)nondet_uint64_t();
    void **val = &out_ptr;

    /* 4. Save old state */
    struct aws_array_list old = list;
    void *old_out_ptr = out_ptr;

    /* 5. Call the function under test */
    int result = aws_array_list_get_at_ptr(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success guarantees: index is within bounds and *val points to the element */
        assert(list.length > index);
        void *expected = (void *)((uint8_t *)list.data + (list.item_size * index));
        assert(out_ptr == expected);
    } else {
        /* On failure the output pointer must remain unchanged */
        assert(out_ptr == old_out_ptr);
    }

    /* 7. Unchanged fields of the list */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 8. Validity invariant must still hold */
    assert(aws_array_list_is_valid(&list));
}
