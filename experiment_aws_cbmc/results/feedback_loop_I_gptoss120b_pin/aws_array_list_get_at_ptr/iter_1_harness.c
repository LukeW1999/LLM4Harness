#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_get_at_ptr_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state of the list */
    struct aws_array_list old = list;

    /* 3. Prepare output pointer */
    void *val = nondet_uint8_t() ? NULL : (void *)0; /* nondet initial value */
    void *old_val = val;

    /* 4. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 5. Call function under test */
    int result = aws_array_list_get_at_ptr(&list, &val, index);

    /* 6. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: index must be within bounds */
        assert(list.length > index);
        /* val points to the correct element */
        assert(val == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        /* Failure: val must be unchanged */
        assert(val == old_val);
    }

    /* 7. Unchanged fields of the list (list is const) */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
