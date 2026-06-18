#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                                MAX_INITIAL_ITEM_ALLOCATION,
                                                MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer for the value to set */
    uint8_t *val_buf = NULL;
    if (list.item_size > 0) {
        val_buf = malloc(list.item_size);
        __CPROVER_assume(val_buf != NULL);
    }
    const void *val = val_buf;

    /* 3. Non‑deterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* 5. Save a byte from the backing array (if any) for immutability checks */
    struct store_byte_from_buffer old_byte;
    bool have_saved_byte = false;
    if (list.data && list.current_size > 0) {
        size_t byte_index = nondet_size_t();
        __CPROVER_assume(byte_index < list.current_size);
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
        have_saved_byte = true;
    }

    /* 6. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 7. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length may increase to index+1 if index was beyond the old length */
        size_t expected_length = old.length;
        if (index >= old.length) {
            expected_length = index + 1;
        }
        assert(list.length == expected_length);

        /* Fields that must not change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size >= old.current_size); /* capacity may grow */

        /* The data at the target index must now equal the source value */
        if (list.data) {
            assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                               (const uint8_t *)val,
                               list.item_size);
        }

        /* Bytes outside the modified element must remain unchanged */
        if (have_saved_byte && list.data) {
            size_t modified_start = index * list.item_size;
            size_t modified_end   = modified_start + list.item_size;
            if (old_byte.offset < modified_start || old_byte.offset >= modified_end) {
                assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
            }
        }
    } else {
        /* Failure path: the function returns AWS_OP_ERR */
        assert(result == AWS_OP_ERR);

        /* The list must be unchanged */
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* Backing store must be unchanged */
        if (have_saved_byte && list.data) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }

    /* 8. The list must always remain valid */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val_buf);
}
