#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_ring_buffer_acquire_harness() {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring_buf;
    size_t init_size = nondet_size_t();
    __CPROVER_assume(init_size > 0);
    __CPROVER_assume(init_size <= MAX_BUFFER_SIZE);
    struct aws_allocator *allocator = aws_default_allocator();
    int init_res = aws_ring_buffer_init(&ring_buf, allocator, init_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Save old state of the ring buffer */
    struct aws_ring_buffer old_ring = ring_buf;
    uint8_t *old_head; AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring, old_head);
    uint8_t *old_tail; AWS_ATOMIC_LOAD_TAIL_PTR(&old_ring, old_tail);

    /* Destination buffer */
    struct aws_byte_buf dest = {0};
    struct aws_byte_buf old_dest = dest; /* save for the failure case */

    /* Requested size (precondition: must be non‑zero and fit in the buffer) */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(requested_size <= init_size); /* enough space for success */

    /* 2. Call the function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Load the new atomic pointers */
    uint8_t *new_head; AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
    uint8_t *new_tail; AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);

    /* 3. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest must describe a freshly vended buffer */
        assert(dest.len == 0);
        assert(dest.capacity == requested_size);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);

        /* head must have moved forward (or wrapped) */
        assert(new_head != old_head);

        /* fields that never change */
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
    } else {
        /* on failure the ring buffer must be unchanged */
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        assert(new_head == old_head);
        assert(new_tail == old_tail);

        /* dest must be unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
    }

    /* 4. Invariant must hold after the call */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
