#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_get_at_ptr_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* Output pointer and its old value */
    void *out = NULL;
    void *old_out = out;

    /* Nondeterministic index */
    size_t index = nondet_size_t();

    /* 3. Call function under test */
    int result = aws_array_list_get_at_ptr(&list, &out, index);

    /* 4. Postconditions for both success and failure */
    if (result == AWS_OP_SUCCESS) {
        /* Success: index is within bounds and out points to the correct element */
        assert(list.length > index);
        assert(out == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        /* Failure: error returned, out unchanged, and index out of bounds */
        assert(result == AWS_OP_ERR);
        assert(out == old_out);
        assert(!(list.length > index));
    }

    /* 5. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 6. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
