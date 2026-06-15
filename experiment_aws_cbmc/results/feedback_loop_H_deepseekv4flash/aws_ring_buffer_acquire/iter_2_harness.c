#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_ring_buffer_acquire_harness() {
    /* Non-deterministic inputs */
    struct aws_ring_buffer ring_buf;
    size_t requested_size;
    struct aws_byte_buf dest;

    /* Assume ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Assume requested_size is non-zero (precondition from implementation) */
    __CPROVER_assume(requested_size > 0);

    /* Ensure dest is initialized (but not necessarily valid) */
    dest.allocator = NULL;
    dest.buffer = NULL;
    dest.capacity = 0;
    dest.len = 0;

    /* Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* Call function */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, dest must be valid and have the requested size */
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
        /* dest.buffer must point within ring_buf's allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer < ring_buf.allocation_end);
        /* dest.allocator should be NULL (from aws_byte_buf_from_empty_array) */
        assert(dest.allocator == NULL);
    } else {
        /* On failure, dest must be unchanged */
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.len == old_dest.len);
    }

    /* Unchanged fields of ring_buf */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    /* Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
