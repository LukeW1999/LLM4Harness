#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_byte_buf_harness() {
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

    struct store_byte_from_buffer old_a_byte;
    struct store_byte_from_buffer old_b_byte;
    save_byte_from_array(a.ptr, a.len, &old_a_byte);
    save_byte_from_array(b.buffer, b.capacity, &old_b_byte);

    bool expected = aws_array_eq(old_a.ptr, old_a.len, old_b.buffer, old_b.len);

    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    assert(result == expected);

    if (result) {
        assert(expected);
        assert(old_a.len == old_b.len);
        assert_bytes_match(old_a.ptr, old_b.buffer, old_a.len);

        assert(a.len == old_a.len);
        assert(a.ptr == old_a.ptr);
        assert(b.len == old_b.len);
        assert(b.buffer == old_b.buffer);
        assert(b.capacity == old_b.capacity);
        assert(b.allocator == old_b.allocator);
    } else {
        assert(!expected);

        assert(a.len == old_a.len);
        assert(a.ptr == old_a.ptr);
        assert(b.len == old_b.len);
        assert(b.buffer == old_b.buffer);
        assert(b.capacity == old_b.capacity);
        assert(b.allocator == old_b.allocator);
    }

    assert_byte_from_buffer_matches(a.ptr, &old_a_byte);
    assert_byte_from_buffer_matches(b.buffer, &old_b_byte);

    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
