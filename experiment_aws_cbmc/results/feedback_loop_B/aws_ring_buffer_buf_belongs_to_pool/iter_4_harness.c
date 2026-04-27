#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Assume ring_buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Assume buf is bounded */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Assume buf is either within or outside the ring buffer's allocation */
    bool belongs_to_pool = nondet_bool();
    if (belongs_to_pool) {
        __CPROVER_assume(buf.buffer >= ring_buffer.allocation && buf.buffer < ring_buffer.allocation_end);
    } else {
        __CPROVER_assume(!(buf.buffer >= ring_buffer.allocation && buf.buffer < ring_buffer.allocation_end));
    }

    /* Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;

    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(result == belongs_to_pool);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.head == old_ring_buffer.head);
    assert(ring_buffer.tail == old_ring_buffer.tail);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    /* 6. Assert validity invariants always holds */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
