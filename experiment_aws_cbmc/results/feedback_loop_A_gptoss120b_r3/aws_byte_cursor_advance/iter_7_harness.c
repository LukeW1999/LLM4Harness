#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/byte_buf.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_cursor_advance_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf src;
    src.allocator = allocator;

    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_cursor cursor = aws_byte_cursor_from_buf(&src);

    struct aws_byte_cursor old_cursor = cursor;
    struct aws_byte_buf old_src = src;

    size_t amount = (size_t)nondet_uint();

    int result = aws_byte_cursor_advance(&cursor, amount);

    if (result == AWS_OP_SUCCESS) {
        assert(cursor.len == old_cursor.len - amount);
        assert(cursor.buffer == (uint8_t *)old_cursor.buffer + amount);
    } else {
        assert(cursor.buffer == old_cursor.buffer);
        assert(cursor.len == old_cursor.len);
    }

    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_cursor_is_valid(&cursor));
}
