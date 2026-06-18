#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state before the call */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((const uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* 2. Non-deterministic index, bounded to avoid overflow */
    size_t index = nondet_size_t();
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Input value – pointer to readable memory of size list.item_size */
    uint8_t val_buf[MAX_ITEM_SIZE];
    const void *val = val_buf;

    /* 4. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Postconditions */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* The element at index must match the input value */
        assert(index < aws_array_list_length(&list));
        assert_bytes_match((const uint8_t *)list.data + (index * list.item_size),
                           (const uint8_t *)val,
                           list.item_size);

        /* length must be at least index + 1 */
        assert(aws_array_list_length(&list) >= index + 1);
    } else {
        /* On failure the list must remain completely unchanged */
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    /* Invariant fields that never change */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
}
