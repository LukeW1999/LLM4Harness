#define MAX_BUFFER_SIZE 1024

#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <string.h>

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Declare and initialise the ring buffer */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);

    int init_res = aws_ring_buffer_init(&ring_buf, allocator, alloc_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);

    /* Ensure the buffer is empty */
    AWS_ATOMIC_STORE_HEAD_PTR(&ring_buf, ring_buf.allocation);
    AWS_ATOMIC_STORE_TAIL_PTR(&ring_buf, ring_buf.allocation);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Declare a destination byte buffer that is initially valid */
    struct aws_byte_buf dest;
    dest.buffer = NULL;
    dest.capacity = 0;
    dest.len = 0;
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* 3. Save old state */
    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* 4. Nondet requested size, bounded, non‑zero and fits in the allocation */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0 && requested_size <= alloc_size);

    /* 5. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest must describe a buffer of the requested size inside the ring allocation */
        assert(dest.len == 0);
        assert(dest.capacity == requested_size);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
    } else {
        /* On failure the ring buffer must be unchanged */
        uint8_t *new_head;
        uint8_t *old_head;
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
        AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring, old_head);
        assert(new_head == old_head);

        uint8_t *new_tail;
        uint8_t *old_tail;
        AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);
        AWS_ATOMIC_LOAD_TAIL_PTR(&old_ring, old_tail);
        assert(new_tail == old_tail);

        /* dest must be unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
    }

    /* 7. Fields that never change */
    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);

    /* 8. Validity invariant */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
}
