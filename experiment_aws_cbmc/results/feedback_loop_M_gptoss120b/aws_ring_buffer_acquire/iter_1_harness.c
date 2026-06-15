#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring_buf;
    /* allocator */
    ring_buf.allocator = aws_default_allocator();

    /* nondet capacity bounded */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0 && capacity <= MAX_BUFFER_SIZE);

    /* allocate the underlying buffer */
    ring_buf.allocation = malloc(capacity);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + capacity;

    /* initialise atomic head and tail to the start of the buffer (empty ring) */
    ring_buf.head = (struct aws_atomic_var){ .value = (void *)ring_buf.allocation };
    ring_buf.tail = (struct aws_atomic_var){ .value = (void *)ring_buf.allocation };

    /* assume the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Declare and bound the destination byte buffer */
    struct aws_byte_buf dest;
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    /* No need to bound dest here; it will be overwritten on success */

    /* 3. Save old state */
    struct aws_ring_buffer old_ring = ring_buf;

    /* 4. Nondet request size, bounded and non‑zero (precondition) */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0 && requested_size <= MAX_BUFFER_SIZE);

    /* 5. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 6. Post‑condition checks */

    /* Success path */
    if (result == AWS_OP_SUCCESS) {
        /* dest must describe a buffer of the requested size */
        assert(dest.len == requested_size);
        /* The buffer must lie within the ring buffer allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        /* The buffer must belong to this ring buffer's pool */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    } else {
        /* On failure the ring buffer must be unchanged */
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        /* head and tail must be unchanged */
        assert(ring_buf.head.value == old_ring.head.value);
        assert(ring_buf.tail.value == old_ring.tail.value);
    }

    /* 7. Fields that never change regardless of outcome */
    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);

    /* 8. Ring buffer validity invariant must hold */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
