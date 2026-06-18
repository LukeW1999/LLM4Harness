#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest = {0};

    /* allocator */
    ring_buf.allocator = aws_default_allocator();

    /* non‑deterministic allocation size, bounded */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);

    ring_buf.allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;

    /* non‑deterministic head and tail positions inside the allocation */
    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= alloc_size);
    __CPROVER_assume(tail_offset <= alloc_size);
    AWS_ATOMIC_STORE_HEAD_PTR(&ring_buf, ring_buf.allocation + head_offset);
    AWS_ATOMIC_STORE_TAIL_PTR(&ring_buf, ring_buf.allocation + tail_offset);

    /* precondition for the function under test */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);

    /* assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state */
    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* 3. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 4. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest must point inside the allocation and have the requested size */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        assert(dest.capacity == requested_size);
        assert(dest.len == requested_size);
    } else {
        /* on failure the ring buffer must be unchanged */
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        assert(ring_buf.allocator == old_ring.allocator);

        uint8_t *old_head, *old_tail, *new_head, *new_tail;
        AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring, old_head);
        AWS_ATOMIC_LOAD_TAIL_PTR(&old_ring, old_tail);
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
        AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);
        assert(new_head == old_head);
        assert(new_tail == old_tail);

        /* dest must be unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.len == old_dest.len);
    }

    /* 5. Fields that never change regardless of result */
    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);

    /* 6. Validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
