#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf = {0};
    struct aws_allocator *allocator = aws_default_allocator();

    /* Ensure we have a valid allocator */
    __CPROVER_assume(allocator != NULL);

    /* Initialise the ring buffer with a non‑zero size */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= 1024);
    __CPROVER_assume(aws_ring_buffer_init(&ring_buffer, allocator, ring_size) == AWS_OP_SUCCESS);

    /* Choose a request size that fits in the ring buffer */
    size_t request = nondet_size_t();
    __CPROVER_assume(request > 0);
    __CPROVER_assume(request <= ring_size);

    /* Load the current tail pointer */
    uint8_t *tail = AWS_ATOMIC_LOAD_PTR(&ring_buffer.tail);

    /* Ensure the request does not wrap past the allocation end */
    __CPROVER_assume(tail + request <= ring_buffer.allocation_end);

    /* Manually construct a buffer that mimics a successful acquire */
    buf.allocator = allocator;
    buf.buffer = tail;
    buf.capacity = request;
    buf.len = request;               /* length may be up to capacity */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

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
