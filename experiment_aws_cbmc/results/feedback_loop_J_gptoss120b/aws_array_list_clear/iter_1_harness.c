#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable buffer for the value to set */
    uint8_t *val_buf = NULL;
    if (list.item_size > 0) {
        val_buf = malloc(list.item_size);
        __CPROVER_assume(val_buf != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(val_buf, list.item_size));
    }

    /* 3. Non‑deterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_storage = {0};
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_storage);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val_buf, index);

    /* 6. Global validity invariant */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* Length change */
        if (index >= old.length) {
            /* Length must become index+1 */
            assert(list.length == index + 1);
        } else {
            /* Length unchanged */
            assert(list.length == old.length);
        }

        /* Data at the targeted index must match the input value */
        assert_bytes_match(
            (uint8_t *)list.data + (index * list.item_size),
            val_buf,
            list.item_size);

        /* Fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* data pointer and current_size may change due to reallocation,
           so they are not asserted here. */
    } else {
        /* On failure the whole structure must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (list.data) {
            assert_byte_from_buffer_matches(list.data, &old_storage);
        }
    }

    /* 7. Clean up */
    free(val_buf);
}
