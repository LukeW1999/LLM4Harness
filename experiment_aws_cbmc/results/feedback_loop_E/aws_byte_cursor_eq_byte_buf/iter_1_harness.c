#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_byte_buf_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_cursor cursor;
    struct aws_byte_buf buffer;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));

    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    ensure_byte_buf_has_allocated_buffer_member(&buffer);

    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_cursor old_cursor = cursor;
    struct aws_byte_buf old_buffer = buffer;

    /* 3. Call function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&cursor, &buffer);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* For aws_byte_cursor_eq_byte_buf, the result is a boolean and doesn't change the inputs */
    if (result) {
        assert(cursor.len == buffer.len);
        assert_bytes_match(cursor.ptr, buffer.buffer, cursor.len);
    } else {
        /* On failure, the cursor and buffer should remain unchanged */
        assert(cursor.len == old_cursor.len);
        assert(cursor.ptr == old_cursor.ptr);
        assert(buffer.len == old_buffer.len);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);
    assert(buffer.len == old_buffer.len);
    assert(buffer.buffer == old_buffer.buffer);
    assert(buffer.capacity == old_buffer.capacity);
    assert(buffer.allocator == old_buffer.allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_buf_is_valid(&buffer));
}
