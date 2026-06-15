#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

/* Helper macro to compute the expected length after a successful set_at */
static size_t expected_length_after_set(size_t old_len, size_t index) {
    return (index >= old_len) ? (index + 1) : old_len;
}

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Constrain item_size to be within the proof bound */
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.item_size > 0);

    /* 3. Prepare a nondeterministic value buffer */
    uint8_t val_buf[MAX_ITEM_SIZE];
    const void *val = (const void *)val_buf;
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 5. Save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte = {0};
    if (old.data != NULL) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    /* 6. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 7. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length may increase to index+1 */
        size_t exp_len = expected_length_after_set(old.length, index);
        assert(list.length == exp_len);

        /* The element at `index` must now equal `val` */
        assert_bytes_match((const uint8_t *)list.data + (index * list.item_size),
                           (const uint8_t *)val,
                           list.item_size);

        /* Fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* If the data pointer is unchanged, the underlying bytes must be unchanged */
        if (list.data != NULL) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    /* 8. The list must always remain valid */
    assert(aws_array_list_is_valid(&list));
}
