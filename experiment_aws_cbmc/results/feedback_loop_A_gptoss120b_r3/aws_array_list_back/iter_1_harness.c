#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate writable output buffer */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* Save the bytes that will be copied on success */
    struct store_byte_from_buffer old_last_elem;
    if (list.length > 0) {
        size_t last_offset = list.item_size * (list.length - 1);
        save_byte_from_array(
            (uint8_t *)list.data + last_offset,
            list.item_size,
            &old_last_elem);
    }

    /* Save the original contents of the output buffer (for the failure case) */
    struct store_byte_from_buffer old_val;
    save_byte_from_array((uint8_t *)val, list.item_size, &old_val);

    /* 4. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        /* The output buffer must contain the former last element */
        if (old.length > 0) {
            size_t last_offset = old.item_size * (old.length - 1);
            assert_bytes_match(
                (uint8_t *)val,
                (uint8_t *)old.data + last_offset,
                old.item_size);
        }
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        /* The output buffer must be unchanged */
        assert_byte_from_buffer_matches((uint8_t *)val, &old_val);
    }

    /* 6. The list must always satisfy its validity invariant */
    assert(aws_array_list_is_valid(&list));
}
