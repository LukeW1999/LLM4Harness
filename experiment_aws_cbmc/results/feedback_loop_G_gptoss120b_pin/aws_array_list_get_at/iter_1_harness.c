#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer for the value to set */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        /* make the buffer readable */
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state before the call */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (old.data && old.current_size > 0) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. The array list must remain valid in all cases */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* ---- Success path postconditions ---- */

        /* allocator and item size never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* length is increased to index+1 if index was beyond the old length */
        {
            size_t expected_len = (index >= old.length) ? (index + 1) : old.length;
            assert(list.length == expected_len);
        }

        /* The element at the given index now matches the supplied value */
        if (list.data && val) {
            assert_bytes_match((uint8_t *)list.data + (list.item_size * index),
                               val,
                               list.item_size);
        }

        /* All other fields of the struct (except those that may change due to reallocation) are unchanged */
        assert(list.current_size >= old.current_size); /* capacity may grow but never shrink */
        /* If the underlying buffer was not reallocated, its pointer stays the same */
        if (old.data != NULL && list.data != NULL && list.current_size == old.current_size) {
            assert(list.data == old.data);
        }
    } else {
        /* ---- Failure path postconditions ---- */

        /* The array list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* If the buffer existed, its contents must be unchanged */
        if (old.data && old.current_size > 0) {
            assert_byte_from_buffer_matches(old.data, &old_byte);
        }
    }
}
