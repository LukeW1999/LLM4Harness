#include <stdbool.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;

    /* completely initialise ring buffer with valid backing store */
    ensure_ring_buffer_has_allocated_members(&ring_buf, MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* set up a valid byte buffer with its own backing store */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* snapshot for immutability checks */
    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf   old_buf  = buf;

    /* call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* post‑conditions on the returned value */
    if (result) {
        /* buffer starts inside the pool and does not exceed it */
        assert(buf.buffer >= ring_buf.allocation);
        assert((buf.buffer + buf.capacity) <= ring_buf.allocation_end);
    } else {
        /* buffer either starts before the pool or extends past its end */
        assert(buf.buffer < ring_buf.allocation ||
               (buf.buffer + buf.capacity) > ring_buf.allocation_end);
    }

    /* immutability: the function must not modify any fields */
    assert(ring_buf.allocator      == old_ring.allocator);
    assert(ring_buf.allocation     == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);
    assert(ring_buf.head           == old_ring.head);
    assert(ring_buf.tail           == old_ring.tail);

    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer    == old_buf.buffer);
    assert(buf.len       == old_buf.len);
    assert(buf.capacity  == old_buf.capacity);

    /* input validity still holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_valid(&buf));
}
