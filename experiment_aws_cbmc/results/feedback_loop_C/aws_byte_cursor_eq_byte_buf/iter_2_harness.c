#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_eq_byte_buf_harness() {
    struct aws_byte_cursor a;
    struct aws_byte_buf b;

    // Initialize a and b with arbitrary values
    size_t max_size = MAX_BUFFER_SIZE;
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);
    a.len = nondet_size_t();
    b.len = nondet_size_t();
    b.capacity = nondet_size_t();
    b.allocator = aws_default_allocator();

    // Ensure a and b are valid initially
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    // Ensure buffers are large enough to avoid out-of-bounds access
    __CPROVER_assume(a.len <= a.buffer->capacity);
    __CPROVER_assume(b.len <= b.capacity);

    // Save old state
    struct aws_byte_cursor old_a = a;
    struct aws_byte_buf old_b = b;

    // Call the function
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    // Assert frame conditions and validity invariants
    assert(a.ptr == old_a.ptr); // UNCHANGED always
    assert(a.len == old_a.len); // UNCHANGED always
    assert(b.buffer == old_b.buffer); // UNCHANGED always
    assert(b.len == old_b.len); // UNCHANGED always
    assert(b.capacity == old_b.capacity); // UNCHANGED always
    assert(b.allocator == old_b.allocator); // UNCHANGED always

    assert(aws_byte_cursor_is_valid(&a)); // aws_byte_cursor_is_valid(&a): YES (must hold after call)
    assert(aws_byte_buf_is_valid(&b)); // aws_byte_buf_is_valid(&b): YES (must hold after call)
}
