#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 16
#endif

void aws_byte_buf_append_harness(void) {
    /* 1. Set up the destination buffer (to) */
    struct aws_byte_buf to;
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&to);
    __CPROVER_assume(aws_byte_buf_is_valid(&to));

    /* 2. Set up the source cursor (from) */
    struct aws_byte_cursor from;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&from);
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* 3. Save old state before the call */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    /* Save a byte from the source cursor for immutability check */
    struct store_byte_from_buffer from_byte_storage;
    if (from.len > 0 && from.ptr != NULL) {
        save_byte_from_array(from.ptr, from.len, &from_byte_storage);
    }

    /* 4. Call the function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* 5. Assert postconditions */

    /* Validity invariants must always hold */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));

    /* Fields of 'to' that must never change */
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);
    assert(to.buffer == old_to.buffer);

    /* Fields of 'from' that must never change */
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    /* Check source cursor byte contents are unchanged */
    if (from.len > 0 && from.ptr != NULL) {
        assert_byte_from_buffer_matches(from.ptr, &from_byte_storage);
    }

    if (result == AWS_OP_SUCCESS) {
        /* On success: to->len increases by from->len */
        assert(to.len == old_to.len + old_from.len);

        /* The data was actually copied: check that to->buffer now contains from->ptr data */
        if (old_from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
        }

        /* to->len must still be <= capacity */
        assert(to.len <= to.capacity);
    } else {
        /* On failure: to->len must be unchanged */
        assert(to.len == old_to.len);

        /* Capacity was too small: to->capacity - to->len < from->len */
        /* (This is the only error path per the implementation) */
        assert(old_to.capacity - old_to.len < old_from.len);
    }
}
