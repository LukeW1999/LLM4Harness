#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_cursor_from_buf_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_buf buf;
    size_t capacity;
    size_t len;

    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_byte_buf_init(&buf, allocator, capacity) == AWS_OP_SUCCESS);

    __CPROVER_assume(len <= capacity);
    buf.len = len;

    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer old_byte;

    if (buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &old_byte);
    }

    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    assert(cur.ptr == old_buf.buffer);
    assert(cur.len == old_buf.len);

    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    if (old_buf.len > 0) {
        assert_byte_from_buffer_matches(buf.buffer, &old_byte);
    }

    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_cursor_is_valid(&cur));

    aws_byte_buf_clean_up(&buf);
}
