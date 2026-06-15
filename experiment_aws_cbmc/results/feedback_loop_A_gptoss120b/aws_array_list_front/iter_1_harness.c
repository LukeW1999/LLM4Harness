#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Non‑deterministic inputs */
    size_t index = nondet_size_t();
    /* bound index to keep the state space finite */
    __CPROVER_assume(index < (MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE));

    uint8_t val_buf[MAX_ITEM_SIZE];
    for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
        val_buf[i] = nondet_uint8_t();
    }
    const void *val = (const void *)val_buf;

    /* Save a byte from the old element (if it existed) for later comparison */
    struct store_byte_from_buffer old_byte;
    if (index < old.length) {
        uint8_t *old_elem = (uint8_t *)old.data + (index * old.item_size);
        save_byte_from_array(old_elem, old.item_size, &old_byte);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Global post‑condition: validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* 6. Fields that never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 7. current_size never decreases */
    assert(list.current_size >= old.current_size);

    if (result == AWS_OP_SUCCESS) {
        /* Length is either unchanged or grows to index+1 */
        size_t expected_len = old.length;
        if (index >= old.length) {
            expected_len = index + 1;
        }
        assert(list.length == expected_len);

        /* The element at `index` now matches the input value */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val_buf,
                           list.item_size);
    } else {
        /* On failure the whole structure must be unchanged */
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
        if (index < old.length) {
            /* The byte we saved from the old element must still be the same */
            uint8_t *elem = (uint8_t *)list.data + (index * list.item_size);
            assert_byte_from_buffer_matches(elem, &old_byte);
        }
    }
}
