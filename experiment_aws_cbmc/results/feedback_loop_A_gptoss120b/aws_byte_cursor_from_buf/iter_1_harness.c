#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_from_buf_harness(void) {
    /* 1. Declare and bound the aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state */
    struct aws_byte_buf old = buf;

    /* 3. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    /* 4. Assert postconditions */
    /* Returned cursor reflects the buffer's contents */
    assert(cur.ptr == old.buffer);
    assert(cur.len == old.len);

    /* 5. Assert unchanged fields of the input buffer */
    assert(buf.buffer == old.buffer);
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);

    /* 6. Assert validity invariants */
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_cursor_is_valid(&cur));
}
