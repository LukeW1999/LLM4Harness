#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

/* Bounding macro – adjust as needed for the proof environment */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet allocation size, bounded */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);

    /* allocate the underlying memory */
    uint8_t *allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(allocation != NULL);
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + alloc_size;
    ring_buf.allocator = alloc;

    /* nondet head and tail offsets within the allocation range */
    size_t head_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= alloc_size);
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(tail_offset <= alloc_size);

    /* For simplicity, keep the buffer empty so acquire can succeed */
    __CPROVER_assume(head_offset == tail_offset);

    /* initialize atomic vars (they hold size_t offsets stored as void*) */
    ring_buf.head = (struct aws_atomic_var){ .value = (void *)head_offset };
    ring_buf.tail = (struct aws_atomic_var){ .value = (void *)tail_offset };

    /* assume the ring buffer is valid before the call */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Declare destination byte buffer */
    struct aws_byte_buf dest;
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;
    dest.allocator = NULL;

    /* Save old state */
    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* 3. Nondet requested size, respecting precondition */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= alloc_size);
    /* Ensure there is enough free space for the request */
    __CPROVER_assume(requested_size <= aws_ring_buffer_capacity(&ring_buf));

    /* 4. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest must point inside the ring buffer allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        /* capacity should match the requested size, length is initially zero */
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
        /* head may have advanced, but tail must be unchanged */
        assert(ring_buf.tail.value == old_ring.tail.value);
    } else {
        /* On failure the ring buffer must be unchanged */
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.head.value == old_ring.head.value);
        assert(ring_buf.tail.value == old_ring.tail.value);

        /* dest must be unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    /* 6. Fields that never change regardless of result */
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);
    assert(ring_buf.allocator == old_ring.allocator);

    /* 7. Validity invariant must hold after the call */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    /* clean up */
    free(allocation);
}
