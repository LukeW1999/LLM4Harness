#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Allocate and initialize a valid ring buffer */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t init_size = nondet_size_t();
    __CPROVER_assume(init_size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(init_size > 0);

    int init_res = aws_ring_buffer_init(&ring_buf, alloc, init_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state */
    struct aws_ring_buffer old_ring = ring_buf;
    uint8_t *old_head;
    uint8_t *old_tail;
    AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring, old_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&old_ring, old_tail);

    /* 3. Prepare inputs */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    struct aws_byte_buf dest;

    /* 4. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest must describe a buffer inside the ring allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);

        /* the acquired buffer must belong to the ring */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));

        /* head must have advanced */
        uint8_t *new_head;
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
        assert(new_head != old_head);

        /* tail must be unchanged */
        uint8_t *new_tail;
        AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);
        assert(new_tail == old_tail);
    } else {
        /* on failure the ring buffer must be unchanged */
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);

        uint8_t *new_head;
        uint8_t *new_tail;
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
        AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);
        assert(new_head == old_head);
        assert(new_tail == old_tail);
    }

    /* 6. Fields that never change */
    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);

    /* 7. Ring buffer must remain valid */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
