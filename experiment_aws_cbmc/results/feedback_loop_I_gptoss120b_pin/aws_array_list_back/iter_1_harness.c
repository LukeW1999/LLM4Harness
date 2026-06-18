#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate writable output buffer */
    void *val = malloc(list.item_size);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* 3. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    /* Save a copy of the output buffer to compare on failure */
    uint8_t *old_val = malloc(list.item_size);
    memcpy(old_val, val, list.item_size);

    /* 4. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success: length was > 0 and the last element was copied */
        size_t last_item_offset = list.item_size * (list.length - 1);
        assert_bytes_match(
            (const uint8_t *)val,
            (const uint8_t *)list.data + last_item_offset,
            list.item_size);
    } else {
        /* Failure: list unchanged and output buffer unchanged */
        assert_bytes_match(old_val, (const uint8_t *)val, list.item_size);
    }

    /* 6. Unchanged fields of the list */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.data == old.data);

    /* 7. Data buffer content unchanged */
    if (list.current_size > 0) {
        assert_byte_from_buffer_matches(list.data, &old_byte);
    }

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
