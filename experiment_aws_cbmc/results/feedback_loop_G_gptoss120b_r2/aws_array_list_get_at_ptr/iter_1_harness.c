#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_get_at_ptr_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    list.alloc = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Prepare output pointer and nondet index */
    void *out_val = NULL;
    void *old_out_val = out_val;               /* save initial value */
    size_t index = nondet_size_t();

    /* Prevent size_t overflow when computing address */
    __CPROVER_assume(old.item_size == 0 || index <= SIZE_MAX / old.item_size);

    /* 4. Call function under test */
    int result = aws_array_list_get_at_ptr(&list, &out_val, index);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success: index must be within bounds and pointer must be correct */
        assert(old.length > index);
        void *expected = (void *)((uint8_t *)old.data + (old.item_size * index));
        assert(out_val == expected);
    } else {
        /* Failure: output pointer must be unchanged and index out of bounds */
        assert(out_val == old_out_val);
        assert(!(old.length > index));
    }

    /* 6. Unchanged fields of the list (list is const) */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
