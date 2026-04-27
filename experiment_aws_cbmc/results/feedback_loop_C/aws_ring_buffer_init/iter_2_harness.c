#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <assert.h>

void aws_ring_buffer_init_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator allocator;
    size_t size = nondet_size_t();
    struct aws_ring_buffer old_ring_buf = ring_buf;

    int result = aws_ring_buffer_init(&ring_buf, &allocator, size);

    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf.allocator == &allocator);
        assert(ring_buf.allocation != NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
    } else {
        assert(ring_buf.allocator == old_ring_buf.allocator);
        assert(ring_buf.allocation == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == aws_atomic_load_ptr(&old_ring_buf.head));
        assert(aws_atomic_load_ptr(&ring_buf.tail) == aws_atomic_load_ptr(&old_ring_buf.tail));
        assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
    }

    assert(aws_ring_buffer_is_valid(&ring_buf));
}
