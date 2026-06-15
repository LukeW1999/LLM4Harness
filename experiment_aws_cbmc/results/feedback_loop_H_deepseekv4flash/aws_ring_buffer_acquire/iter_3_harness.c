#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_ring_buffer_acquire_harness() {
    struct aws_ring_buffer ring_buf;
    size_t requested_size;
    struct aws_byte_buf dest;

    /* Initialize ring buffer with non-deterministic valid storage */
    make_ring_buffer_has_allocated_storage(&ring_buf);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* requested_size must be > 0 (precondition) */
    __CPROVER_assume(requested_size > 0);

    /* Initialize dest to a non-deterministic (but valid) byte buf */
    make_byte_buf(&dest);

    /* Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* Call function */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer < ring_buf.allocation_end);
        assert(dest.allocator == NULL);
    } else {
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.len == old_dest.len);
    }

    /* Unchanged ring buffer fields */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
