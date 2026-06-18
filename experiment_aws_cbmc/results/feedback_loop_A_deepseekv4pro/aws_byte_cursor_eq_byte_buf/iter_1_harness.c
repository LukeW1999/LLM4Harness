#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_byte_buf_harness() {
    /* declare and bound the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    buf.allocator = aws_default_allocator();
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* save old state (function is read-only, but we assert unchanged anyway) */
    struct aws_byte_cursor old_cursor = cursor;
    struct aws_byte_buf old_buf = buf;

    /* call function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&cursor, &buf);

    /* postcondition: structures must remain unchanged */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* validity invariants still hold */
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_buf_is_valid(&buf));

    /* functional correctness: result reflects content equality */
    if (cursor.len == 0 && buf.len == 0) {
        /* two empty buffers / cursors => always equal */
        assert(result == true);
    } else if (cursor.len != buf.len) {
        /* lengths differ => not equal */
        assert(result == false);
    } else {
        /* lengths match and are > 0 => compare bytes */
        assert(result == (memcmp(cursor.ptr, buf.buffer, cursor.len) == 0));
    }
}
