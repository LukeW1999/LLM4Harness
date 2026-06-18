#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness() {
    /* 1. Allocate and initialize a valid ring buffer */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t init_size = nondet_size_t();
    __CPROVER_assume(init_size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(init_size > 0);               /* non‑zero allocation for meaningful test */

    int init_res = aws_ring_buffer_init(&ring_buf, alloc, init_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Save old atomic head/tail values */
    uint8_t *old_head = NULL;
    uint8_t *old_tail = NULL;
    AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, old_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, old_tail);

    /* 2. Prepare destination byte buffer */
    struct aws_byte_buf dest;
    /* dest is overwritten on success; its previous contents are irrelevant */

    /* Save old state of the whole structures */
    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* 3. Nondeterministic requested size (precondition: non‑zero) */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    /* 4. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 5. Load new atomic head/tail values for comparison */
    uint8_t *new_head = NULL;
    uint8_t *new_tail = NULL;
    AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest points inside the ring buffer allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer < ring_buf.allocation_end);
        /* enough space remains from dest.buffer for the requested size */
        assert((size_t)(ring_buf.allocation_end - (uint8_t *)dest.buffer) >= requested_size);

        /* head must have advanced (or wrapped) */
        assert(new_head != old_head);
        /* tail may change only in the empty‑buffer case; no specific requirement here */

        /* fields that must stay unchanged */
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
        assert(dest.allocator == old_dest.allocator);
    }

    /* 7. Invariant must hold after the call */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
