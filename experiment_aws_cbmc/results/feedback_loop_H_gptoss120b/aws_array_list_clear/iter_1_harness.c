#include <aws/common/array_list.h>
#include <aws/common/assert.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Prepare nondeterministic inputs */
    size_t index = nondet_size_t();
    /* Ensure the index is within a reasonable range for the model checker */
    __CPROVER_assume(index <= (MAX_INITIAL_ITEM_ALLOCATION * 2));

    /* Allocate a nondeterministic value buffer of size list.item_size */
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    uint8_t val_buf[MAX_ITEM_SIZE];
    /* Make the buffer readable for list.item_size bytes */
    __CPROVER_assume(AWS_MEM_IS_READABLE(val_buf, list.item_size));

    /* 4. Call the function under test */
    int result = aws_array_list_set_at(&list, val_buf, index);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length is updated to max(old.length, index+1) */
        size_t expected_length = (index >= old.length) ? (index + 1) : old.length;
        assert(list.length == expected_length);

        /* item_size and allocator never change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* current_size may grow but never shrink */
        assert(list.current_size >= old.current_size);

        /* The stored bytes at the target index must equal the input value */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val_buf,
                           list.item_size);
    } else {
        /* On failure the list must remain unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    }

    /* 6. Invariant must hold regardless of outcome */
    assert(aws_array_list_is_valid(&list));
}
