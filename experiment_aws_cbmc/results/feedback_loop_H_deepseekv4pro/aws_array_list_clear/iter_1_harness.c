#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministically choose index and allocate readable val buffer */
    size_t index = nondet_size_t();
    size_t item_size = list.item_size;

    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Save old state BEFORE calling */
    struct aws_array_list old = list;
    struct store_byte_from_buffer val_storage;
    save_byte_from_array((const uint8_t *)val, item_size, &val_storage);

    /* 4. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Assert postconditions */

    /* Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* alloc and item_size must never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        /* The val bytes must be copied into the array at position index */
        assert(index * item_size + item_size <= list.current_size);
        assert_bytes_match((const uint8_t *)list.data + (index * item_size),
                           (const uint8_t *)val, item_size);

        /* Length update: if index was within existing elements, length unchanged.
         * Otherwise, length becomes index + 1. */
        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            assert(list.length == index + 1);
        }
    }

    free(val);
}
