#include <aws/common/array_list.h>
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

    /* 3. Prepare a readable input buffer */
    uint8_t val_buf[MAX_ITEM_SIZE];
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    /* make the buffer readable */
    __CPROVER_assume(AWS_MEM_IS_READABLE(val_buf, list.item_size));

    /* 4. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val_buf, index);

    /* 6. Global post‑condition: list must remain valid */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* 6.1. The element at the given index now matches the input */
        uint8_t *dest = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(dest, val_buf, list.item_size);

        /* 6.2. Length updates only when index is beyond the old length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* 6.3. Fields that must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* data pointer and current_size may change due to reallocation,
           so we do not assert them here */
    } else {
        /* 7. On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
    }
}
