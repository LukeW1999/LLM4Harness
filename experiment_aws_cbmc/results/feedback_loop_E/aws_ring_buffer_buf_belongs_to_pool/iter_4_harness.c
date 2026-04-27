#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* Non-deterministic parameters. */
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;

    /* Preconditions. */
    __CPROVER_assume(aws_ring_buffer_is_bounded(&ring_buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_buf = buf;

    /* Operation under verification. */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* Postconditions. */
    if (result) {
        assert(buf.buffer >= ring_buf.allocation && buf.buffer < ring_buf.allocation_end);
    } else {
        assert(buf.buffer != NULL || buf.buffer == NULL); // Ensure buffer is either valid or NULL
        assert(buf.len <= buf.capacity);
    }

    /* Assert fields that must NOT change regardless of result */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.head == old_ring_buf.head);
    assert(ring_buf.tail == old_ring_buf.tail);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_valid(&buf));
}
