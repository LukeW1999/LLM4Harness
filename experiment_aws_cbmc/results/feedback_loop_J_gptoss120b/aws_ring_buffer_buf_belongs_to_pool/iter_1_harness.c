#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring;
    struct aws_byte_buf buf;

    /* allocator must be a valid pointer */
    ring.allocator = aws_default_allocator();

    /* ensure the byte buffer has an allocated backing buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* bound the byte buffer to keep the state space finite */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* assume the ring buffer satisfies its invariant */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* 2. Save old state before calling */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* 4. Assert unchanged fields (function is pure) */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head == old_ring.head);
    assert(ring.tail == old_ring.tail);

    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 5. Validity invariants must still hold */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 6. Logical post‑condition derived from the specification */
    if (result) {
        /* The buffer must lie within the ring buffer's allocation region */
        assert(buf.buffer >= ring.allocation);
        assert(buf.buffer < ring.allocation_end);
    }
}
