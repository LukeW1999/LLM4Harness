#include <assert.h>
#include <stddef.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_cursor_advance_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_cursor cursor = aws_byte_cursor_from_buf(&src);
    struct aws_byte_cursor old_cursor = cursor;

    size_t amount; /* nondet */

    int result = aws_byte_cursor_advance(&cursor, amount);

    assert(src.buffer == ((uint8_t *)old_cursor.ptr - old_cursor.len));
    assert(src.len == old_cursor.len + ((uint8_t *)old_cursor.ptr - src.buffer));
    assert(aws_byte_buf_is_valid(&src));

    if (result == AWS_OP_SUCCESS) {
        assert(amount <= old_cursor.len);
        assert(cursor.ptr == old_cursor.ptr + amount);
        assert(cursor.len == old_cursor.len - amount);
    } else {
        assert(amount > old_cursor.len);
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    assert(aws_byte_cursor_is_valid(&cursor));
}
