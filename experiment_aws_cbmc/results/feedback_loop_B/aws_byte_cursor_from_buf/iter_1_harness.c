#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_from_buf_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old = buf;

    /* 3. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* This function always succeeds and directly assigns values, so we only need success path */
    assert(cur.ptr == buf.buffer);
    assert(cur.len == buf.len);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(buf.allocator == old.allocator);
    assert(buf.capacity == old.capacity);
    assert(buf.buffer == old.buffer);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_cursor_is_valid(&cur));
}
