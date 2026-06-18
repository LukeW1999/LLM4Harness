#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_append_harness() {
    /* 1. Declare and bound the destination buffer */
    struct aws_byte_buf to;
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&to);
    __CPROVER_assume(aws_byte_buf_is_valid(&to));

    /* 2. Declare and bound the source cursor */
    struct aws_byte_cursor from;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&from);
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* 3. Save old state before calling */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    /* Save a byte from the source cursor for immutability check */
    struct store_byte_from_buffer from_byte;
    if (from.len > 0 && from.ptr != NULL) {
        save_byte_from_array(from.ptr, from.len, &from_byte);
    }

    /* 4. Call function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* 5. Assert postconditions */

    /* Fields that must NEVER change regardless of result */
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);
    assert(to.buffer == old_to.buffer);

    /* Cursor fields must never change */
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    if (result == AWS_OP_SUCCESS) {
        /* On success: dest->len contains the amount of data actually copied */
        assert(to.len == old_to.len + from.len);

        /* The data was actually copied: verify bytes match if from is non-empty */
        if (from.len > 0 && from.ptr != NULL && to.buffer != NULL) {
            assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
        }
    } else {
        /* On failure (AWS_ERROR_DEST_COPY_TOO_SMALL): to is unchanged */
        assert(to.len == old_to.len);

        /* The condition that caused failure */
        assert(to.capacity - old_to.len < from.len);
    }

    /* 6. Assert validity invariants always hold after the call */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));

    /* Verify source cursor data was not modified */
    if (from.len > 0 && from.ptr != NULL) {
        assert_byte_from_buffer_matches(from.ptr, &from_byte);
    }
}
