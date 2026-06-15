#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_ring_buffer_release_harness() {
    /* Non-deterministic ring buffer */
    struct aws_ring_buffer *ring_buffer = malloc(sizeof(*ring_buffer));
    if (ring_buffer == NULL) return;

    /* Non-deterministic buffer */
    struct aws_byte_buf *buf = malloc(sizeof(*buf));
    if (buf == NULL) {
        free(ring_buffer);
        return;
    }

    /* Ensure ring buffer is valid */
    ring_buffer->allocator = nondet_ptr();
    ring_buffer->allocation = nonderef_malloc(1); /* non-null */
    ring_buffer->allocation_end = ring_buffer->allocation + nondet_size_t();
    __CPROVER_assume(ring_buffer->allocation < ring_buffer->allocation_end);
    /* Initialize head and tail to zero (or any value) */
    aws_atomic_store_int(&ring_buffer->head, 0);
    aws_atomic_store_int(&ring_buffer->tail, 0);

    /* Setup buffer to point within the ring buffer allocation */
    size_t offset = nondet_size_t();
    __CPROVER_assume(offset <= (size_t)(ring_buffer->allocation_end - ring_buffer->allocation));
    buf->buffer = ring_buffer->allocation + offset;

    size_t remaining = (size_t)(ring_buffer->allocation_end - buf->buffer);
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= remaining);
    buf->capacity = capacity;

    size_t len = nondet_size_t();
    __CPROVER_assume(len <= capacity);
    buf->len = len;

    buf->allocator = ring_buffer->allocator; /* set to ring's allocator as common usage */

    /* Save old state for later checks */
    struct aws_ring_buffer old_ring = *ring_buffer;

    /* Call the function under test */
    aws_ring_buffer_release(ring_buffer, buf);

    /* Postcondition: buffer is zeroed */
    assert(buf->len == 0);
    assert(buf->buffer == NULL);
    assert(buf->capacity == 0);
    assert(buf->allocator == NULL);

    /* Unchanged fields of the ring buffer */
    assert(ring_buffer->allocator == old_ring.allocator);
    assert(ring_buffer->allocation == old_ring.allocation);
    assert(ring_buffer->allocation_end == old_ring.allocation_end);

    /* Validity invariant must hold after call */
    assert(aws_ring_buffer_is_valid(ring_buffer));

    /* Free allocated memory */
    free(ring_buffer->allocation);
    free(ring_buffer);
    free(buf);
}
