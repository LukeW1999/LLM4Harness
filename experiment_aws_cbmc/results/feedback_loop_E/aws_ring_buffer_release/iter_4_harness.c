#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_release_harness() {
    /* Non-deterministic parameters. */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Preconditions. */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(buf.buffer >= ring_buffer.allocation && buf.buffer < ring_buffer.allocation_end);
    __CPROVER_assume(buf.buffer + buf.capacity <= ring_buffer.allocation_end);

    /* Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* Operation under verification. */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Postconditions. */
    if (aws_ring_buffer_is_valid(&ring_buffer)) {
        /* Fields that change on success. */
        assert(AWS_ATOMIC_LOAD_INT(&ring_buffer.tail) == (int)(old_buf.buffer + old_buf.capacity - ring_buffer.allocation));

        /* Fields that should be unchanged on success. */
        assert(ring_buffer.allocator == old_ring_buffer.allocator);
        assert(ring_buffer.allocation == old_ring_buffer.allocation);
        assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
        assert(ring_buffer.head == old_ring_buffer.head);

        /* Check that buf is zeroed out on success. */
        assert(buf.buffer == NULL);
        assert(buf.capacity == 0);
        assert(buf.len == 0);
    } else {
        /* Fields that should be unchanged on failure. */
        assert(ring_buffer.allocator == old_ring_buffer.allocator);
        assert(ring_buffer.allocation == old_ring_buffer.allocation);
        assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
        assert(ring_buffer.head == old_ring_buffer.head);
        assert(ring_buffer.tail == old_ring_buffer.tail);
        assert(buf.buffer == old_buf.buffer);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.len == old_buf.len);
    }

    /* Validity invariants. */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
