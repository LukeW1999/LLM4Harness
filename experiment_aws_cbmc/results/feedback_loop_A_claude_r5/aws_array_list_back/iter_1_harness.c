#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate val buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Save old state before calling */
    struct aws_array_list old = list;

    /* Save a byte from the data buffer for immutability check */
    struct store_byte_from_buffer saved_byte;
    if (list.current_size > 0 && list.data != NULL) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &saved_byte);
    }

    /* 4. Call function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Assert postconditions */

    /* The list itself must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* All list fields must be unchanged (back is a read-only operation) */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    if (result == AWS_OP_SUCCESS) {
        /* Success: list was non-empty, val now contains the last element */
        assert(list.length > 0);

        /* The data in the list should be unchanged */
        if (list.current_size > 0 && list.data != NULL) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &saved_byte);
        }

        /* val should contain a copy of the last element */
        /* The last element is at offset item_size * (length - 1) */
        size_t last_item_offset = list.item_size * (list.length - 1);
        assert_bytes_match((uint8_t *)val,
                           (uint8_t *)list.data + last_item_offset,
                           list.item_size);
    } else {
        /* Failure: list was empty */
        assert(list.length == 0);

        /* All fields unchanged on failure */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    }
}
