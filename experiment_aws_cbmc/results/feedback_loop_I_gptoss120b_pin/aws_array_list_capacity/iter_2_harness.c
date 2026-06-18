#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non‑deterministic index */
    size_t index = nondet_size_t();

    /* 3. Allocate a readable source buffer for the value to set */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    /* 4. Save old state for later comparison */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte = {0};
    if (old.data != NULL && old.current_size > 0) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. The array list must always remain valid */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* ----- Success path post‑conditions ----- */

        /* item size and allocator never change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* length either stays the same or grows to index+1 */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* current size never shrinks */
        assert(list.current_size >= old.current_size);

        /* The element at the given index now matches the source value */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);

        /* All other bytes remain unchanged (except possibly those overwritten by a reallocation) */
        assert_array_list_equivalence(&list, &old, &old_byte);
    } else {
        /* ----- Failure path post‑conditions ----- */

        /* The structure must be unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* The previously saved byte must still match */
        if (old.data != NULL && old.current_size > 0) {
            assert_byte_from_buffer_matches(old.data, &old_byte);
        }
    }

    /* Clean up the allocated source buffer */
    if (val != NULL) {
        free(val);
    }
}
