#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_get_at_ptr_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 3. Output pointer */
    void *out = NULL;
    void **val = &out;

    /* 4. Save old state */
    struct aws_array_list old = list;
    void *old_out = out;

    /* 5. Call function under test */
    int result = aws_array_list_get_at_ptr(&list, val, index);

    /* 6. Postconditions for both success and failure */
    if (result == AWS_OP_SUCCESS) {
        /* Success: index must be within bounds */
        assert(index < old.length);
        /* *val must point to the correct element */
        void *expected = (void *)((uint8_t *)list.data + (list.item_size * index));
        assert(*val == expected);
    } else {
        /* Failure: index out of bounds */
        assert(index >= old.length);
        /* *val must remain unchanged */
        assert(out == old_out);
    }

    /* 7. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
