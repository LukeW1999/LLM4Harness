#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_cursor_advance_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();
    buf.allocator = allocator;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf.len <= buf.capacity);
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_cursor cursor = aws_byte_cursor_from_buf(&buf);

    struct aws_byte_cursor old_cursor = cursor;
    struct aws_byte_buf old_buf = buf;

    size_t advance;
    __CPROVER_assume(advance <= old_cursor.len);

    int result = aws_byte_cursor_advance(&cursor, advance);

    if (result == AWS_OP_SUCCESS) {
        assert(cursor.ptr == old_cursor.ptr + advance);
        assert(cursor.len == old_cursor.len - advance);
    } else {
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(aws_byte_buf_is_valid(&buf));

    assert(aws_byte_cursor_is_valid(&cursor));
}
