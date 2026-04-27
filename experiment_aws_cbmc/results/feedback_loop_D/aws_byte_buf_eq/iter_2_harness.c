#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_eq_harness() {
    struct aws_byte_buf old_a, old_b;
    struct aws_byte_buf a;
    struct aws_byte_buf b;

    ensure_byte_buf_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);

    old_a = a;
    old_b = b;

    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    __CPROVER_assume(a.buffer != NULL);
    __CPROVER_assume(b.buffer != NULL);

    bool result = aws_byte_buf_eq(&a, &b);

    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    assert(a.buffer == old_a.buffer);
    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(a.allocator == old_a.allocator);

    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);
}
