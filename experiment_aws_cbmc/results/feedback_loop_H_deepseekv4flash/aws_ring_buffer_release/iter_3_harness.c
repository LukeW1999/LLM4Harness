#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_ring_buffer_release_harness() {
    /* Allocate ring buffer */
    struct aws_ring_buffer *ring_buffer = malloc(sizeof(*ring_buffer));
    if (ring_buffer == NULL) return;

    /* Allocate buffer to release */
    struct aws_byte_buf *buf = malloc(sizeof(*buf));
    if (buf == NULL) {
        free(ring_buffer);
        return;
    }

    /* Set up ring buffer allocation */
    ring_buffer->allocation = nonderef_malloc(1); /* non-null */
    ring_buffer->allocation_end = ring_buffer->allocation + nondet_size_t();
    __CPROVER_assume(ring_buffer->allocation < ring_buffer->allocation_end);

    /* Allocate a valid allocator (non-null) */
    ring_buffer->allocator = (struct aws_allocator *)nonderef_malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(ring_buffer->allocator != NULL);

    /* Set head and tail as offsets (size_t) within allocation range.
     * We set them to a non-deterministic offset that is valid. */
    size_t head_offset, tail_offset;
    __CPROVER_assume(head_offset < (size_t)(ring_buffer->allocation_end - ring_buffer->allocation));
    __CPROVER_assume(tail_offset < (size_t)(ring_buffer->allocation_end - ring_buffer->allocation));
    aws_atomic_store_int(&ring_buffer->head, head_offset);
    aws_atomic_store_int(&ring_buffer->tail, tail_offset);

    /* Ensure ring buffer is valid before the call */
    __CPROVER_assume(aws_ring_buffer_is_valid(ring_buffer));

    /* Set up buf to point within the ring buffer allocation */
    size_t offset;
    __CPROVER_assume(offset < (size_t)(ring_buffer->allocation_end - ring_buffer->allocation));
    buf->buffer = ring_buffer->allocation + offset;

    size_t remaining = (size_t)(ring_buffer->allocation_end - buf->buffer);
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= remaining);
    buf->capacity = capacity;

    size_t len = nondet_size_t();
    __CPROVER_assume(len <= buf->capacity);
    buf->len = len;

    buf->allocator = ring_buffer->allocator; /* matches ring's allocator */

    /* Save old ring buffer state for unchanged field checks */
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

    /* Ring buffer must remain valid after release */
    assert(aws_ring_buffer_is_valid(ring_buffer));

    /* Clean up */
    free(ring_buffer->allocation);
    free(ring_buffer->allocator);
    free(ring_buffer);
    free(buf);
}
