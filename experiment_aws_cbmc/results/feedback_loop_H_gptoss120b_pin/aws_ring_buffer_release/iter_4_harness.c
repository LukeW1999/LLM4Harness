#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Choose a non‑zero ring buffer size */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= 1024);

    /* Initialise the ring buffer */
    __CPROVER_assume(aws_ring_buffer_init(&ring_buffer, allocator, ring_size) == AWS_OP_SUCCESS);

    /* Acquire a buffer from the ring buffer */
    size_t request = nondet_size_t();
    __CPROVER_assume(request <= ring_size);
    __CPROVER_assume(request > 0);
    __CPROVER_assume(aws_ring_buffer_acquire(&ring_buffer, &buf, request) == AWS_OP_SUCCESS);

    /* Set a length for the buffer (may be less than the capacity) */
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    /* Save old state for post‑condition checks */
    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf old_buf = buf;
    uint8_t *old_head = AWS_ATOMIC_LOAD_PTR(&ring_buffer.head);
    uint8_t *old_tail = AWS_ATOMIC_LOAD_PTR(&ring_buffer.tail);

    /* Call the function under verification */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Verify that the buffer has been cleared */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* Verify that the ring buffer state is updated correctly */
    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);
    assert(AWS_ATOMIC_LOAD_PTR(&ring_buffer.head) == old_head);
    assert(AWS_ATOMIC_LOAD_PTR(&ring_buffer.tail) == old_tail + old_buf.len);

    /* The ring buffer must remain valid after the release */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
