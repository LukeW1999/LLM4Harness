#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_acquire_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t requested_size;
    struct aws_byte_buf dest;
    int result;

    // Initialize the ring buffer
    ring_buf.allocator = allocator;
    ring_buf.allocation = (uint8_t *)bounded_malloc(MAX_BUFFER_SIZE);
    ring_buf.allocation_end = ring_buf.allocation + MAX_BUFFER_SIZE;
    AWS_ATOMIC_INIT_VAR(ring_buf.head, ring_buf.allocation);
    AWS_ATOMIC_INIT_VAR(ring_buf.tail, ring_buf.allocation);

    // Initialize the destination byte buffer
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;

    // Non-deterministic requested size
    requested_size = nondet_size_t();
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    // Save old state
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_dest = dest;

    // Call the function under test
    result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    // Step 1: Assertions for success path
    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf.head.var != old_ring_buf.head.var);
        assert(dest.buffer != old_dest.buffer);
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);
        if (old_ring_buf.head.var == old_ring_buf.tail.var) {
            assert(ring_buf.tail.var != old_ring_buf.tail.var);
        }
    }

    // Step 2: Assertions for failure path
    if (result != AWS_OP_SUCCESS) {
        assert(ring_buf.head.var == old_ring_buf.head.var);
        assert(ring_buf.tail.var == old_ring_buf.tail.var);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
    }

    // Step 3: Frame conditions
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    // Step 4: Validity invariants
    assert(aws_ring_buffer_is_valid(&ring_buf));
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_bounded(&dest, requested_size));
    }
}
