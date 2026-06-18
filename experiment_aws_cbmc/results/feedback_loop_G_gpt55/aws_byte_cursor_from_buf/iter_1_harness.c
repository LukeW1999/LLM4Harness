#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_buf_harness() {
    struct aws_byte_buf buf;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;

    struct aws_byte_cursor result = aws_byte_cursor_from_buf(&buf);

    assert(result.ptr == old.buffer);
    assert(result.len == old.len);

    assert(buf.len == old.len);
    assert(buf.buffer == old.buffer);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);

    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_cursor_is_valid(&result));
}
