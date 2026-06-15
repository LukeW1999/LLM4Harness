#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <aws/common/ring_buffer.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024

size_t nondet_size_t(void);
bool nondet_bool(void);

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;

    /* allocate ring buffer memory */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);
    ring_buf.allocation = malloc(alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = (uint8_t *)ring_buf.allocation + alloc_size;
    ring_buf.allocator = NULL; /* allocator not used in this harness */

    /* nondet head and tail within allocation */
    size_t head_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= alloc_size);
    ring_buf.head = (uint8_t *)ring_buf.allocation + head_offset;

    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(tail_offset <= alloc_size);
    ring_buf.tail = (uint8_t *)ring_buf.allocation + tail_offset;

    /* ensure the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_byte_buf dest = {0};

    /* save old state */
    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* nondet requested size */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0 && requested_size <= MAX_BUFFER_SIZE);

    /* compute available contiguous space (simplified) */
    size_t available;
    if (ring_buf.head <= ring_buf.tail) {
        available = (size_t)(ring_buf.allocation_end - ring_buf.tail);
    } else {
        available = (size_t)(ring_buf.head - ring_buf.tail);
    }

    /* make both success and failure paths reachable */
    if (nondet_bool()) {
        __CPROVER_assume(requested_size <= available);
    } else {
        __CPROVER_assume(requested_size > available);
    }

    /* call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer >= (uint8_t *)ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= (uint8_t *)ring_buf.allocation_end);
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    } else {
        assert(ring_buf.head == old_ring.head);
        assert(ring_buf.tail == old_ring.tail);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        assert(ring_buf.allocator == old_ring.allocator);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
    }

    /* fields that never change */
    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);

    /* invariant must hold */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
