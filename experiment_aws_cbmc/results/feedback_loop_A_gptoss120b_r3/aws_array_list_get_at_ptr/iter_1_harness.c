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
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                              MAX_INITIAL_ITEM_ALLOCATION,
                                              MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Declare the output pointer variable */
    void *out_ptr = NULL;
    void **val = &out_ptr;

    /* 3. Save old state of the list and the output pointer */
    struct aws_array_list old = list;
    void *old_out = out_ptr;

    /* 4. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 5. Call the function under test */
    int result = aws_array_list_get_at_ptr(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success guarantees: index is within bounds and the pointer is computed correctly */
        assert(aws_array_list_length(&list) > index);
        assert(*val == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        /* Failure guarantees: index is out of bounds and the output pointer is unchanged */
        assert(aws_array_list_length(&list) <= index);
        assert(out_ptr == old_out);
    }

    /* 7. Unchanged fields of the list (the function is read‑only) */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 8. The list must remain valid */
    assert(aws_array_list_is_valid(&list));
}
