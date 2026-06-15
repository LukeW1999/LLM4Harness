#include <assert.h>
#include <stddef.h>
#include <aws/common/ring_buffer.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_byte_buf dest;
    /* dest will be overwritten on success; no need to allocate its buffer beforehand */

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* 3. Non‑deterministic input with preconditions */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    /* 4. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest must describe a buffer of the requested size inside the ring allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);

        /* the returned buffer must belong to the ring buffer's pool */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    } else {
        /* on failure the ring buffer must be unchanged */
        assert(ring_buf.head == old_ring.head);
        assert(ring_buf.tail == old_ring.tail);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        assert(ring_buf.allocator == old_ring.allocator);

        /* dest must be unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
    }

    /* 6. Fields that never change regardless of outcome */
    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);

    /* 7. Validity invariant must always hold */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
