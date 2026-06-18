#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                                MAX_INITIAL_ITEM_ALLOCATION,
                                                MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non‑deterministic index (bounded to avoid overflow in arithmetic) */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE);
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    /* 3. Non‑deterministic value to write */
    uint8_t val_buf[MAX_ITEM_SIZE];
    for (size_t i = 0; i < list.item_size; ++i) {
        val_buf[i] = nondet_uint8_t();
    }
    const void *val = val_buf;

    /* 4. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (list.data) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_bytes);
    }

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑conditions */
    if (result == AWS_OP_SUCCESS) {
        /* data at index must match the supplied value */
        assert(list.data != NULL);
        uint8_t *dest = (uint8_t *)list.data + (index * list.item_size);
        assert_bytes_match(dest, (const uint8_t *)val, list.item_size);

        /* length updates if we wrote past the previous end */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* fields that must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* unchanged bytes outside the modified element */
        assert_array_list_equivalence(&list, &old, &old_bytes);
    } else {
        /* on failure the list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* unchanged bytes */
        assert_array_list_equivalence(&list, &old, &old_bytes);
    }

    /* 7. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
