#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness() {
    /* Non‑deterministic inputs */
    struct aws_ring_buffer ring;
    struct aws_byte_buf buf;

    /* Preconditions: ring buffer and byte buffer are valid,
       and the byte buffer's memory region lies within the ring buffer's allocation. */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(buf.buffer >= ring.allocation && buf.buffer + buf.capacity <= ring.allocation_end);

    /* Save old state of the ring buffer (byte buffer will be zeroed) */
    struct aws_ring_buffer old_ring = ring;

    /* Call the function under test */
    aws_ring_buffer_release(&ring, &buf);

    /* Postconditions */
    /* 1. Ring buffer is still valid */
    assert(aws_ring_buffer_is_valid(&ring));

    /* 2. Byte buffer is zeroed */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* 3. Ring buffer fields that must not change */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    /* head and tail are atomic; we cannot safely compare them directly,
       but the ring buffer validity check above already ensures they remain
       within the allocation bounds. */
}
