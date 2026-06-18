#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Snapshot old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte = {0};
    if (list.data && list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    /* 3. Nondeterministic inputs */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* the contents of *val are nondeterministic – no need to initialise */

    size_t index = nondet_size_t();

    /* 4. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Global invariant must hold */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* ----- fields that may change on success ----- */
        /* length may grow to index+1 if index was beyond the old length */
        size_t expected_length = old.length;
        if (index >= old.length) {
            expected_length = index + 1;
        }
        assert(list.length == expected_length);

        /* the element at the given index must now equal the supplied value */
        if (list.data) {
            assert_bytes_match(
                (uint8_t *)list.data + (index * list.item_size),
                val,
                list.item_size);
        }

        /* ----- fields that must stay unchanged ----- */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* If the underlying buffer was not reallocated, the bytes that were not
         * overwritten must remain unchanged. */
        if (list.data == old.data && list.current_size > 0) {
            /* The saved byte must still be the same unless it lies inside the
             * overwritten region. */
            size_t overwritten_start = index * list.item_size;
            size_t overwritten_end   = overwritten_start + list.item_size;
            if (old_byte.offset < overwritten_start ||
                old_byte.offset >= overwritten_end) {
                assert_byte_from_buffer_matches(list.data, &old_byte);
            }
        }
    } else {
        /* ----- on failure the list must be unchanged ----- */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        if (list.data && list.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    /* 6. Clean‑up */
    free(val);
}
