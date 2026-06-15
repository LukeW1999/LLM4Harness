#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_buf_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;

    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    assert(cur.ptr == buf.buffer);
    assert(cur.len == buf.len);

    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.buffer == old.buffer);
    assert(buf.allocator == old.allocator);

    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_cursor_is_valid(&cur));
}
