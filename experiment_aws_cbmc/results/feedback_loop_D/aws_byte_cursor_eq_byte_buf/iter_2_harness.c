#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_eq_byte_buf_harness() {
    struct aws_byte_cursor a;
    struct aws_byte_buf b;

    // Ensure a and b have allocated buffer members
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);

    // Assume b is valid
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    // Save old state
    struct aws_byte_cursor old_a = a;
    struct aws_byte_buf old_b = b;

    // Call the function
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    // Assert frame conditions
    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);
    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);

    // Assert validity invariants
    assert(aws_byte_cursor_is_valid(&a));
}
