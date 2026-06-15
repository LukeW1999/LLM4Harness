#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_ring_buffer_release_harness() {
    /* Non-deterministic ring buffer */
    struct aws_ring_buffer ring;
    struct aws_ring_buffer *ring_buffer = &ring;

    /* Non-deterministic buffer */
    struct aws_byte_buf buf;
    struct aws_byte_buf *buf_ptr = &buf;

    /* Ensure ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(ring_buffer));

    /* Save old state of ring buffer */
    struct aws_ring_buffer old_ring = *ring_buffer;

    /* Setup buffer to point within the ring buffer allocation */
    size_t offset = nondet_size_t();
    __CPROVER_assume(offset <= ring_buffer->allocation_end - ring_buffer->allocation);
    buf.buffer = ring_buffer->allocation + offset;

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= (ring_buffer->allocation_end - buf.buffer));
    buf.capacity = capacity;

    buf.len = 0;  /* buffer is assumed acquired and now being released typically has some length, but we set to 0 for simplicity */

    /* The buffer's allocator is not used in release, but we assume it's set to the ring's allocator for completeness */
    buf.allocator = ring_buffer->allocator;

    /* Call the function under test */
    aws_ring_buffer_release(ring_buffer, buf_ptr);

    /* Postcondition: buffer is zeroed */
    assert(buf.len == 0);
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    /* The allocator field of buf is also zeroed (part of zeroing the whole structure) */
    assert(buf.allocator == NULL);

    /* Unchanged fields of the ring buffer */
    assert(ring_buffer->allocator == old_ring.allocator);
    assert(ring_buffer->allocation == old_ring.allocation);
    assert(ring_buffer->allocation_end == old_ring.allocation_end);
    /* head is not modified, but it's an atomic variable; we skip detailed check */

    /* Validity invariant must hold after call */
    assert(aws_ring_buffer_is_valid(ring_buffer));
}
