#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_release_harness() {
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    // Initialize ring_buffer with arbitrary values
    ring_buffer.allocator = (struct aws_allocator *)nondet_ptr();
    ring_buffer.allocation = (uint8_t *)nondet_ptr();
    ring_buffer.head = (struct aws_atomic_var){.var = nondet_uint64_t()};
    ring_buffer.tail = (struct aws_atomic_var){.var = nondet_uint64_t()};
    ring_buffer.allocation_end = (uint8_t *)nondet_ptr();

    // Initialize buf with arbitrary values
    buf.buffer = (uint8_t *)nondet_ptr();
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    buf.allocator = (struct aws_allocator *)nondet_ptr();

    // Ensure the buffer is within the allocation range
    assume(buf.buffer >= ring_buffer.allocation && buf.buffer + buf.capacity <= ring_buffer.allocation_end);

    // Save old state
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    // Call the function under test
    int result = aws_ring_buffer_release(&ring_buffer, &buf);

    // Assert frame conditions and validity invariants
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.head.var == old_ring_buffer.head.var);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == old_buf.allocator);

    if (result == AWS_OP_SUCCESS) {
        // Tail should change on success
        assert(ring_buffer.tail.var == (uintptr_t)(old_buf.buffer + old_buf.capacity));
    } else {
        // Tail should remain unchanged on failure
        assert(ring_buffer.tail.var == old_ring_buffer.tail.var);
    }

    // Check validity invariants
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
