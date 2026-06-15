#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_cursor_advance_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_cursor cursor = {
        .ptr = buf.buffer,
        .len = buf.len
    };

    const uint8_t *old_ptr = cursor.ptr;
    size_t old_len = cursor.len;
    struct aws_byte_buf old_buf = buf;

    size_t amount = nondet_size_t();
    __CPROVER_assume(amount <= MAX_BUFFER_SIZE);

    int result = aws_byte_cursor_advance(&cursor, amount);

    if (result == AWS_OP_SUCCESS) {
        assert(amount <= old_len);
        assert(cursor.ptr == old_ptr + amount);
        assert(cursor.len == old_len - amount);
    } else {
        assert(cursor.ptr == old_ptr);
        assert(cursor.len == old_len);
    }

    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    assert(aws_byte_buf_is_valid(&buf));

    assert(
        (cursor.ptr == NULL && cursor.len == 0) ||
        (cursor.ptr >= buf.buffer &&
         cursor.ptr <= buf.buffer + buf.len &&
         cursor.len <= (size_t)(buf.buffer + buf.len - cursor.ptr))
    );
}
