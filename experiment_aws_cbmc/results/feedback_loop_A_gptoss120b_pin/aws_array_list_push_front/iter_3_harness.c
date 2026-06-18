#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Use the default allocator */
    list.alloc = aws_default_allocator();
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable input value of size list.item_size */
    size_t item_sz = list.item_size;
    __CPROVER_assume(item_sz > 0);
    uint8_t *val_buf = malloc(item_sz);
    __CPROVER_assume(val_buf != NULL);
    for (size_t i = 0; i < item_sz; ++i) {
        val_buf[i] = nondet_uint8_t();
    }

    /* 3. Save old state before the call */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data_byte = {0};
    if (old.data != NULL && old.length > 0) {
        save_byte_from_array(
            (uint8_t *)old.data,
            old.length * old.item_size,
            &old_data_byte);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val_buf);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length must increase by one */
        assert(list.length == old.length + 1);
        /* the new front element must equal the supplied value */
        assert_bytes_match((uint8_t *)list.data, val_buf, list.item_size);
    } else {
        /* on failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.data != NULL && old.length > 0) {
            assert_byte_from_buffer_matches(
                (uint8_t *)list.data,
                &old_data_byte);
        }
    }

    /* Fields that never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 6. Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val_buf);
}
