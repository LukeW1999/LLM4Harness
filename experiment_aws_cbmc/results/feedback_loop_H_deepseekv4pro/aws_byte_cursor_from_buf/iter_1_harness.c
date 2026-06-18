#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_buf_harness() {
    struct aws_byte_buf buf;

    /* Bound the input buffer size and ensure a valid, allocated buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;

    /* Function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_buf(&buf);

    /* Input buffer must remain completely unchanged */
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.buffer == old.buffer);
    assert(buf.allocator == old.allocator);
    assert(aws_byte_buf_is_valid(&buf));

    /* Cursor must point to the buffer's data and have the same length */
    assert(cursor.ptr == buf.buffer);
    assert(cursor.len == buf.len);

    /* The resulting cursor must be valid */
    assert(aws_byte_cursor_is_valid(&cursor));
}
