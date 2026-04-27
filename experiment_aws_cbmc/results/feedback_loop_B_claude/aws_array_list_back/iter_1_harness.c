#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "aws/common/array_list.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a writable output buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Save old state before calling */
    struct aws_array_list old = list;

    /* Save a byte from the backing data for immutability check */
    struct store_byte_from_buffer saved_byte;
    if (list.current_size > 0 && list.data != NULL) {
        save_byte_from_array((const uint8_t *)list.data, list.current_size, &saved_byte);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Assert postconditions */

    /* The list structure itself must never be modified by this function */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* The backing data must not be modified */
    if (list.current_size > 0 && list.data != NULL) {
        assert_byte_from_buffer_matches((const uint8_t *)list.data, &saved_byte);
    }

    if (result == AWS_OP_SUCCESS) {
        /* Success path: list was non-empty */
        assert(list.length > 0);

        /* val should now contain the last element — verify by checking
         * that val matches the last item in the backing store */
        size_t last_item_offset = list.item_size * (list.length - 1);
        assert_bytes_match(
            (const uint8_t *)val,
            (const uint8_t *)list.data + last_item_offset,
            list.item_size);
    } else {
        /* Failure path: list was empty */
        assert(list.length == 0);
        /* val is unchanged — we cannot assert its contents since it was
         * non-deterministic, but we can assert the list is still valid */
    }

    /* 6. Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
