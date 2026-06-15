#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_cursor_eq_byte_buf_harness() {
    /* Non-deterministic data structures */
    struct aws_byte_cursor cursor;
    struct aws_byte_buf buf;

    /* Assumptions about bounds */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate buffer members */
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume validity */
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state for immutability checks */
    struct aws_byte_cursor old_cursor = cursor;
    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer cursor_storage;
    if (cursor.len > 0) {
        save_byte_from_array(cursor.ptr, cursor.len, &cursor_storage);
    }
    struct store_byte_from_buffer buf_storage;
    if (buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &buf_storage);
    }

    /* Call the function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&cursor, &buf);

    /* Assert fields of cursor unchanged */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);
    if (cursor.len > 0) {
        assert_byte_from_buffer_matches(cursor.ptr, &cursor_storage);
    }

    /* Assert fields of buffer unchanged */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    if (buf.len > 0) {
        assert_byte_from_buffer_matches(buf.buffer, &buf_storage);
    }

    /* Assert validity invariants */
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_buf_is_valid(&buf));

    /* If lengths differ, the result must be false (arrays cannot be equal) */
    if (old_cursor.len != old_buf.len) {
        assert(result == false);
    }
}
