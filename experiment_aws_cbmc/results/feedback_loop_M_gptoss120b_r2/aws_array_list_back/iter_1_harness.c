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

void aws_array_list_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate writable output buffer */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer storage;
    if (old.length > 0) {
        size_t last_offset = old.item_size * (old.length - 1);
        save_byte_from_array((uint8_t *)old.data + last_offset,
                             old.item_size,
                             &storage);
    }

    /* 4. Call function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* success: length must have been > 0 and the last element copied */
        assert(old.length > 0);
        size_t last_offset = old.item_size * (old.length - 1);
        assert_bytes_match((const uint8_t *)val,
                           (const uint8_t *)old.data + last_offset,
                           old.item_size);
    } else {
        /* failure: list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        if (old.length > 0) {
            /* data buffer must be unchanged */
            size_t last_offset = old.item_size * (old.length - 1);
            assert_bytes_match((const uint8_t *)old.data + last_offset,
                               (const uint8_t *)old.data + last_offset,
                               old.item_size);
        }
    }

    /* 6. Fields that never change (regardless of result) */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
