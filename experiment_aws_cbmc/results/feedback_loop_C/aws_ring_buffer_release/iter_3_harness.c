#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    // Initialize ring_buffer and buf with arbitrary values
    ring_buffer.allocator = allocator;
    ring_buffer.allocation = (uint8_t *)nondet_ptr();
    ring_buffer.head = (struct aws_atomic_var){.var = nondet_uintptr_t()};
    ring_buffer.tail = (struct aws_atomic_var){.var = nondet_uintptr_t()};
    ring_buffer.allocation_end = (uint8_t *)nondet_ptr();
    buf.buffer = (uint8_t *)nondet_ptr();
    buf.capacity = nondet_size_t();
    buf.len = nondet_size_t();

    // Save old state
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    // Assume preconditions
    assume(aws_ring_buffer_is_valid(&ring_buffer));
    assume(aws_byte_buf_is_bounded(&buf, SIZE_MAX));
    assume(buf.buffer >= ring_buffer.allocation && buf.buffer <= ring_buffer.allocation_end);
    assume(buf.buffer + buf.capacity <= ring_buffer.allocation_end);

    // Call the function under test
    aws_ring_buffer_release(&ring_buffer, &buf);

    // Assert frame conditions and postconditions
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.head.var == old_ring_buffer.head.var);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert(aws_ring_buffer_is_valid(&ring_buffer));

    if (buf.buffer == old_buf.buffer && buf.capacity == old_buf.capacity) {
        // Failure path
        assert(ring_buffer.tail.var == old_ring_buffer.tail.var);
        assert(buf.buffer == old_buf.buffer);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.len == old_buf.len);
    } else {
        // Success path
        assert(ring_buffer.tail.var == (uintptr_t)(old_buf.buffer + old_buf.capacity));
        assert(buf.buffer == NULL);
        assert(buf.capacity == 0);
        assert(buf.len == 0);
    }
}
