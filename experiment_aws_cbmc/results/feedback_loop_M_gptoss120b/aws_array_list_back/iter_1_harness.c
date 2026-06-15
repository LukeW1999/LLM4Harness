#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a writable buffer for the output value */
    uint8_t val_buf[MAX_ITEM_SIZE];
    void *val = (void *)val_buf;

    /* 3. Save old state of the list */
    struct aws_array_list old = list;

    /* 4. Save the bytes that may be copied into val on success */
    uint8_t expected_bytes[MAX_ITEM_SIZE];
    if (list.length > 0) {
        size_t last_offset = list.item_size * (list.length - 1);
        memcpy(expected_bytes,
               (uint8_t *)list.data + last_offset,
               list.item_size);
    }

    /* 5. Save the original contents of val (to check unchanged on failure) */
    uint8_t original_val[MAX_ITEM_SIZE];
    memcpy(original_val, val_buf, MAX_ITEM_SIZE);

    /* 6. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 7. Post‑condition checks */

    /* ---- Success path ---- */
    if (result == AWS_OP_SUCCESS) {
        /* 7.1. The list must have been non‑empty */
        assert(list.length > 0);

        /* 7.2. No fields of the list change */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        /* 7.3. The output buffer now contains the last element */
        assert_bytes_match((uint8_t *)val,
                           expected_bytes,
                           list.item_size);
    } else {
        /* ---- Failure path (list empty) ---- */

        /* 7.4. The list is unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        /* 7.5. The output buffer is unchanged */
        assert_bytes_match((uint8_t *)val,
                           original_val,
                           MAX_ITEM_SIZE);
    }

    /* 8. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
