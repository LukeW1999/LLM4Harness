#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_byte_buf_harness(void) {
    struct aws_byte_cursor a;
    struct aws_byte_buf b;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));

    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    struct aws_byte_cursor old_a = a;
    struct aws_byte_buf old_b = b;

    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);

    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);

    bool expected = aws_array_eq(old_a.ptr, old_a.len, old_b.buffer, old_b.len);
    assert(result == expected);

    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
