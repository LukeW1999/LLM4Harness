#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;
    size_t max_size = MAX_BUFFER_SIZE;

    /* Bound the ring buffer and buffer */
    __CPROVER_assume(aws_ring_buffer_is_bounded(&ring_buffer, max_size));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, max_size));

    /* Allocate necessary memory */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume ring buffer and byte buffer are valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Assume buf.buffer points within ring_buffer.allocation and ring_buffer.allocation_end */
    __CPROVER_assume(buf.buffer >= ring_buffer.allocation && buf.buffer <= ring_buffer.allocation_end - buf.capacity);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* In this function, there is no failure path, so we only need to check success conditions */
    assert(ring_buffer.tail == old_ring_buffer.tail); // tail should not change
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert(ring_buffer.head == old_ring_buffer.head);

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
}
