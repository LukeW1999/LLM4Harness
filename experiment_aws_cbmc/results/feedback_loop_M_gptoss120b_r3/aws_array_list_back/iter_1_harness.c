#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
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

    /* 2. Save old state for immutability checks */
    struct aws_array_list old = list;

    /* 3. Allocate a writable buffer for the output value */
    uint8_t val_buf[MAX_ITEM_SIZE];
    void *val = (void *)val_buf;

    /* 4. Save the bytes of the last element (if any) for later comparison */
    struct store_byte_from_buffer old_last_elem;
    if (list.length > 0) {
        size_t last_offset = list.item_size * (list.length - 1);
        __CPROVER_assume(last_offset + list.item_size <= list.current_size);
        save_byte_from_array(
            (const uint8_t *)list.data + last_offset,
            list.item_size,
            &old_last_elem);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        /* The output buffer must contain the last element */
        if (old.length > 0) {
            size_t last_offset = old.item_size * (old.length - 1);
            assert_bytes_match(
                (const uint8_t *)val,
                (const uint8_t *)old.data + last_offset,
                old.item_size);
        }
    } else {
        /* On failure the list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        /* The output buffer must not be modified */
        assert(val == val_buf);
    }

    /* 7. The list must still satisfy its validity predicate */
    assert(aws_array_list_is_valid(&list));
}
