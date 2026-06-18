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

    /* 3. Prepare output pointer */
    void *out = NULL;
    void **val = &out;
    void *old_out = out; /* capture initial value of *val */

    /* 4. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 5. Call function under test */
    int result = aws_array_list_get_at_ptr(&list, val, index);

    /* 6. Postconditions for success and failure */
    if (result == AWS_OP_SUCCESS) {
        /* Success guarantees: index is within bounds and *val points to the element */
        assert(list.length > index);
        assert(out == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        /* On failure, *val must remain unchanged */
        assert(out == old_out);
    }

    /* 7. Fields that must remain unchanged regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 8. Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
