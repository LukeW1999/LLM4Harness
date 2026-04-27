#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_cursor_eq_byte_buf_harness(void) {
    /* 1. Declare and set up aws_byte_cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Declare and set up aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 3. Save old state before calling */
    struct aws_byte_cursor old_cursor = cursor;
    struct aws_byte_buf old_buf = buf;

    /* 4. Call function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&cursor, &buf);

    /* 5. Assert postconditions */

    /* The function compares contents of cursor and buf.
     * It returns true if their contents are equivalent, false otherwise.
     * Neither the cursor nor the buf should be modified by this function. */

    /* Cursor fields must be unchanged */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);

    /* Buf fields must be unchanged */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* Validity invariants must still hold */
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_buf_is_valid(&buf));

    /* Semantic postcondition:
     * result is true iff the contents of cursor and buf are equivalent.
     * If lengths differ, result must be false. */
    if (cursor.len != buf.len) {
        assert(result == false);
    }

    /* If result is true, lengths must be equal */
    if (result) {
        assert(cursor.len == buf.len);
    }
}
