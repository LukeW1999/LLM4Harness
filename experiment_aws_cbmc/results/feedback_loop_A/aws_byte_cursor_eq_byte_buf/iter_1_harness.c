#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_eq_byte_buf_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_cursor cursor;
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_cursor old_cursor = cursor;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&cursor, &buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Since the function returns a boolean, there are no separate success/failure paths to handle */
    assert(result == aws_array_eq(cursor.ptr, cursor.len, buf.buffer, buf.len));

    /* 5. Assert fields that must NOT change regardless of result */
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_buf_is_valid(&buf));
}
