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

    /* 2. Prepare the output pointer */
    void *output = (void *)nondet_uint64_t();   /* nondet initial value */
    void **val = &output;                       /* val itself is never NULL */

    /* 3. Save old state */
    struct aws_array_list old = list;
    void *old_output = output;

    /* 4. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 5. Call the function under test */
    int result = aws_array_list_get_at_ptr(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* On success the pointer is set to the address of the indexed element */
        void *expected = (void *)((uint8_t *)list.data + (list.item_size * index));
        assert(output == expected);
    } else {
        /* On failure the output pointer must remain unchanged */
        assert(output == old_output);
    }

    /* 7. Fields that must remain unchanged regardless of the result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 8. Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
