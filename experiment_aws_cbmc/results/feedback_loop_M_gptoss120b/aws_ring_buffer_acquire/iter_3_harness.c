#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0 && capacity <= 1024);

    ring_buf.allocation = (uint8_t *)aws_mem_acquire(allocator, capacity);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + capacity;
    ring_buf.size = capacity;

    aws_atomic_init_ptr(&ring_buf.head, ring_buf.allocation);
    aws_atomic_init_ptr(&ring_buf.tail, ring_buf.allocation);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_byte_buf dest;
    ensure_byte_buf_has_allocated_buffer_member(&dest);

    struct aws_ring_buffer old_ring = ring_buf;

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0 && requested_size <= 1024);

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == requested_size);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    } else {
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        assert(aws_atomic_load_ptr(&ring_buf.head) == aws_atomic_load_ptr(&old_ring.head));
        assert(aws_atomic_load_ptr(&ring_buf.tail) == aws_atomic_load_ptr(&old_ring.tail));
    }

    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);
    assert(ring_buf.size == old_ring.size);

    assert(aws_ring_buffer_is_valid(&ring_buf));
}
