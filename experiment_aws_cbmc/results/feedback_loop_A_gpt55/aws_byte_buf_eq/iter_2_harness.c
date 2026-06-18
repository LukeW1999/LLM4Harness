#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_eq_harness() {
    struct aws_byte_buf a;
    struct aws_byte_buf b;

    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_buf_is_valid(&a));

    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    struct store_byte_from_buffer old_a_byte;
    struct store_byte_from_buffer old_b_byte;
    save_byte_from_array(a.buffer, a.len, &old_a_byte);
    save_byte_from_array(b.buffer, b.len, &old_b_byte);

    bool result = aws_byte_buf_eq(&a, &b);

    if (a.len != b.len) {
        assert(!result);
    }

    if (result) {
        assert(a.len == b.len);
        assert_bytes_match(a.buffer, b.buffer, a.len);
    }

    assert(a.allocator == old_a.allocator);
    assert(a.buffer == old_a.buffer);
    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert_byte_from_buffer_matches(a.buffer, &old_a_byte);
    assert(aws_byte_buf_is_valid(&a));

    assert(b.allocator == old_b.allocator);
    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert_byte_from_buffer_matches(b.buffer, &old_b_byte);
    assert(aws_byte_buf_is_valid(&b));
}
