#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_erase_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* Save a byte from the data for later comparison if needed */
    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0 && list.data != NULL) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* 3. Non-deterministic index */
    size_t index;

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: index must have been valid (< length) */
        /* length decreases by 1 */
        assert(list.length == old.length - 1);
        /* item_size unchanged */
        assert(list.item_size == old.item_size);
        /* alloc unchanged */
        assert(list.alloc == old.alloc);
        /* current_size unchanged (erase doesn't reallocate) */
        assert(list.current_size == old.current_size);
        /* data pointer unchanged */
        assert(list.data == old.data);
    } else {
        /* On failure: index >= length, list is unchanged */
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        /* Verify the byte in the buffer is unchanged */
        if (old.current_size > 0 && old.data != NULL) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }

    /* 6. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
