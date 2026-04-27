#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Declare and bound data structures */
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;
    size_t max_buffer_size = 1024; // Define a reasonable max buffer size for testing
    __CPROVER_assume(aws_ring_buffer_is_bounded(&ring_buffer, max_buffer_size));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, max_buffer_size));
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Simulate allocation within the ring buffer for the byte buffer */
    __CPROVER_assume(buf.buffer >= ring_buffer.allocation && buf.buffer + buf.capacity <= ring_buffer.allocation_end);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert(buf.buffer >= ring_buffer.allocation && buf.buffer < ring_buffer.allocation_end);
    } else {
        assert(buf.buffer < ring_buffer.allocation || buf.buffer >= ring_buffer.allocation_end);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.head == old_ring_buffer.head);
    assert(ring_buffer.tail == old_ring_buffer.tail);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
}
