#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>

#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;

    /* allocator */
    ring_buf.allocator = aws_default_allocator();

    /* allocation */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
    ring_buf.allocation = alloc_size ? malloc(alloc_size) : NULL;
    __CPROVER_assume(alloc_size == 0 || ring_buf.allocation != NULL);
    ring_buf.allocation_end = alloc_size ? ring_buf.allocation + alloc_size : NULL;

    /* atomic head and tail – left nondeterministic, validity will be enforced by the predicate */
    /* (no explicit initialization needed) */

    /* 2. Assume ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Save old state */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Non‑deterministic request size, respecting the precondition */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    /* 4. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest describes a newly acquired slice */
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
        assert(dest.buffer != NULL);
        assert(ring_buf.allocation != NULL);                 /* allocation must exist when success */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
    } else {
        /* on failure the ring buffer must be unchanged */
        assert(ring_buf.head == old.head);
        assert(ring_buf.tail == old.tail);
    }

    /* Fields that never change */
    assert(ring_buf.allocator == old.allocator);
    assert(ring_buf.allocation == old.allocation);
    assert(ring_buf.allocation_end == old.allocation_end);

    /* 6. Validity invariant holds after the call */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
