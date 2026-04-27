#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_acquire_harness() {
    /* Non-deterministic parameters. */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t requested_size;
    struct aws_byte_buf dest;

    /* Preconditions. */
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(requested_size > 0 && requested_size < MAX_MALLOC);
    ensure_allocated_ring_buffer(&ring_buf, allocator, requested_size);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    __CPROVER_assume(AWS_MEM_IS_READABLE(ring_buf.allocation, ring_buf.allocation_end - ring_buf.allocation));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(ring_buf.allocation, ring_buf.allocation_end - ring_buf.allocation));

    /* Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buf = ring_buf;

    /* Operation under verification. */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Postconditions. */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_ring_buffer_is_valid(&ring_buf));
        assert(dest.buffer != NULL);
        assert(dest.len == requested_size);
        assert(dest.capacity >= requested_size);
        assert(AWS_MEM_IS_WRITABLE(dest.buffer, dest.capacity));
        assert(dest.buffer >= ring_buf.allocation && dest.buffer < ring_buf.allocation_end);
        assert((ring_buf.head.var >= old_ring_buf.head.var && ring_buf.head.var <= ring_buf.allocation_end - ring_buf.allocation) ||
               (ring_buf.head.var >= ring_buf.allocation - ring_buf.allocation && ring_buf.head.var < old_ring_buf.head.var));
    } else {
        assert(aws_ring_buffer_is_valid(&ring_buf));
        assert(ring_buf.head.var == old_ring_buf.head.var);
        assert(ring_buf.tail.var == old_ring_buf.tail.var);
        assert(ring_buf.allocation == old_ring_buf.allocation);
        assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
        assert(ring_buf.allocator == old_ring_buf.allocator);
    }

    /* Fields that must NOT change regardless of result */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
}
