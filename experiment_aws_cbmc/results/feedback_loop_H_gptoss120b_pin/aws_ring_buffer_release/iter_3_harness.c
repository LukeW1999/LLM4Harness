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

    /* Allocate a non‑zero ring buffer */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= 1024);
    ring_buffer.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;
    ring_buffer.allocator = allocator;

    /* Initialise head and tail to the start of the allocation */
    AWS_ATOMIC_STORE_PTR(&ring_buffer.head, ring_buffer.allocation);
    AWS_ATOMIC_STORE_PTR(&ring_buffer.tail, ring_buffer.allocation);

    /* Create a buffer that starts exactly at the current tail */
    size_t offset = 0;                                   /* must equal tail offset */
    size_t cap = nondet_size_t();
    __CPROVER_assume(cap <= ring_size - offset);
    buf.buffer = ring_buffer.allocation + offset;
    buf.capacity = cap;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= cap);
    buf.allocator = NULL;                               /* acquire returns a buffer without an allocator */

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

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
