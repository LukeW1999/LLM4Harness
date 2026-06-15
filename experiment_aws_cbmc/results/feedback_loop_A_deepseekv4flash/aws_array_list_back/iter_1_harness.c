#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"
#include "aws/common/array_list.h"

void aws_array_list_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data);
    }

    /* 3. Prepare val buffer */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* 4. Call function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: list->length > 0, last element copied to val */
        assert(list.length > 0);
        size_t last_item_offset = list.item_size * (list.length - 1);
        assert_bytes_match(val, (uint8_t *)list.data + last_item_offset, list.item_size);
    } else {
        /* Failure: list is empty, list unchanged */
        assert(list.length == 0);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        if (list.data != NULL && list.current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_data);
        }
    }

    /* 6. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    /* length unchanged on failure, but on success length is unchanged (back doesn't modify length) */
    assert(list.length == old.length);

    /* 7. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    free(val);
}
