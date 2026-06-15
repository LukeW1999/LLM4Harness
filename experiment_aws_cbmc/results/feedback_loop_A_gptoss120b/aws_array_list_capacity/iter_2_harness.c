#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Prepare a readable input value */
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    uint8_t val_buf[MAX_ITEM_SIZE];
    for (size_t i = 0; i < list.item_size; ++i) {
        val_buf[i] = nondet_uint8_t();
    }

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data_byte = {0};
    if (old.length > 0 && old.data != NULL) {
        save_byte_from_array((uint8_t *)old.data, old.current_size, &old_data_byte);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val_buf, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* the element at `index` must now contain the bytes from `val_buf` */
        assert_bytes_match(
            (uint8_t *)list.data + (index * list.item_size),
            val_buf,
            list.item_size);

        /* length is increased only when writing past the previous end */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* fields that must not change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    } else {
        /* on failure the list must be unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.length == old.length);

        if (old.length > 0 && old.data != NULL) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_data_byte);
        }
    }

    /* 7. The list must always remain valid */
    assert(aws_array_list_is_valid(&list));
}
