#include <assert.h>
#include <stddef.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_cursor cursor = {
        .ptr = buf.buffer,
        .len = buf.len
    };

    struct aws_byte_cursor old_cursor = cursor;

    size_t amount;
    __CPROVER_assume(amount <= MAX_BUFFER_SIZE);

    int result = aws_byte_cursor_advance(&cursor, amount);

    assert(aws_byte_cursor_is_valid(&cursor));

    if (result == AWS_OP_SUCCESS) {
        assert(cursor.len == old_cursor.len - amount);
        if (amount > 0) {
            assert(old_cursor.ptr != NULL);
            assert(cursor.ptr == old_cursor.ptr + amount);
        } else {
            assert(cursor.ptr == old_cursor.ptr);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    assert(buf.buffer == old_cursor.ptr);
    assert(buf.len == old_cursor.len);
    assert(buf.capacity == buf.capacity);
    assert(buf.allocator == alloc);
}
