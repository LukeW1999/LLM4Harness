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

void aws_array_list_push_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable input value */
    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        /* nondet contents */
        uint8_t *bytes = (uint8_t *)val;
        for (size_t i = 0; i < list.item_size; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* Save a byte from the old data buffer for later equivalence checks */
    struct store_byte_from_buffer old_byte;
    if (old.length > 0 && old.item_size > 0) {
        size_t idx = nondet_size_t();
        __CPROVER_assume(idx < old.length);
        save_byte_from_array(
            (uint8_t *)old.data + idx * old.item_size,
            old.item_size,
            &old_byte);
    }

    /* 4. Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length increased by one */
        assert(list.length == old.length + 1);

        /* the new front element matches the supplied value */
        assert_bytes_match(
            (const uint8_t *)list.data,
            (const uint8_t *)val,
            list.item_size);

        /* if there were previous elements, at least one byte of the
           former first element is still present at index 1 */
        if (old.length > 0 && old.item_size > 0) {
            assert_byte_from_buffer_matches(
                (const uint8_t *)list.data + list.item_size,
                &old_byte);
        }
    } else {
        /* on failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 6. Fields that never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
}
