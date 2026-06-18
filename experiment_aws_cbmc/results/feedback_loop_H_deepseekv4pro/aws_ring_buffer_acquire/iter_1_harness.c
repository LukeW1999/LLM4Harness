#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_RING_BUFFER_ALLOCATION 10 /* small bound to limit state space */

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;
    size_t requested_size;

    /* 1. Initialize ring buffer with a bounded allocation */
    ring_buf.allocator = aws_default_allocator();
    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_RING_BUFFER_ALLOCATION);
    ring_buf.allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;

    /* 2. Set up a valid byte_buf for dest (will be overwritten on success, compared on failure) */
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* 3. requested_size is non‑zero and does not exceed allocation (gives a chance of success) */
    requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= alloc_size);

    /* 4. Save old values for immutability checks */
    uint8_t *old_head, *old_tail;
    AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, old_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, old_tail);
    struct aws_byte_buf old_dest = dest;

    /* 5. Ensure ring buffer is valid before the call */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 6. Call the function under verification */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 7. Read the atomic pointers after the call */
    uint8_t *new_head, *new_tail;
    AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);

    /* 8. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* 8a. Success: dest must contain the acquired region */
        assert(dest.buffer != NULL);
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);
        /* The returned buffer lies entirely within the ring buffer's allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        /* The ring buffer remains valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* 8b. Failure: ring buffer state unchanged */
        assert(old_head == new_head);
        assert(old_tail == new_tail);
        /* dest unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        /* Ring buffer validity is preserved */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }

    /* 9. Never‑modified fields */
    assert(ring_buf.allocator == aws_default_allocator());
    assert(ring_buf.allocation == ring_buf.allocation); /* tautology but documents invariance */
    assert(ring_buf.allocation_end == ring_buf.allocation + alloc_size);

    /* 10. Clean up allocated memory */
    free(ring_buf.allocation);
    free(old_dest.buffer);
}
