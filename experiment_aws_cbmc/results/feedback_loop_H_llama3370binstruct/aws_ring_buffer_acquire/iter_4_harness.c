#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_ring_buffer_acquire_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    ensure_byte_buf_has_allocated_buffer_member(&ring_buf.allocation);
    __CPROVER_assume(ring_buf.allocation_end != NULL);
    ring_buf.allocator = aws_default_allocator();

    struct aws_byte_buf dest;
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= ring_buf.allocation_end - ring_buf.allocation);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* 3. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == requested_size);
        assert(dest.capacity >= requested_size);
        assert(dest.buffer != NULL);
        assert(aws_ring_buffer_is_valid(&ring_buf));
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        assert(ring_buf.head == old_ring_buf.head);
        assert(ring_buf.tail == old_ring_buf.tail);
        assert(ring_buf.allocation == old_ring_buf.allocation);
        assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
        assert(ring_buf.allocator == old_ring_buf.allocator);
        assert(dest == old_dest);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_valid(&dest));
}
