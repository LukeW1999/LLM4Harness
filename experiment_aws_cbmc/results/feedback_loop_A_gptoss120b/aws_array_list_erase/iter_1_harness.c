#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_erase_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save a byte from the data buffer for immutability checks (if any) */
    struct store_byte_from_buffer data_byte;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &data_byte);
    }

    /* 2. Save old state before the call */
    struct aws_array_list old = list;

    /* 3. Non-deterministic index */
    size_t index = nondet_size_t();

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Postconditions for success */
    if (result == AWS_OP_SUCCESS) {
        /* Length must decrease by one */
        assert(list.length == old.length - 1);

        /* Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    }

    /* 6. Fields that must never change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
