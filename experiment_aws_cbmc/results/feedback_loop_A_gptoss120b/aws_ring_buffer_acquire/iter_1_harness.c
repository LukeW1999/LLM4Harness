#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>

#include "proof_helpers/make_common_data_structures.h"

void aws_ring_buffer_acquire_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_byte_buf dest;
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* 3. Non‑deterministic input respecting preconditions */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    /* 4. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest points inside the ring buffer allocation and has the requested length */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        assert(dest.len == requested_size);

        /* fields that must not change */
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);

        /* head and tail remain within the allocation bounds */
        assert(ring_buf.head >= (struct aws_atomic_var){ .value = ring_buf.allocation });
        assert(ring_buf.head <= (struct aws_atomic_var){ .value = ring_buf.allocation_end });
        assert(ring_buf.tail >= (struct aws_atomic_var){ .value = ring_buf.allocation });
        assert(ring_buf.tail <= (struct aws_atomic_var){ .value = ring_buf.allocation_end });
    } else {
        /* on failure the ring buffer must be unchanged */
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        assert(ring_buf.head == old_ring.head);
        assert(ring_buf.tail == old_ring.tail);

        /* dest must be unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
    }

    /* 6. Invariant must hold regardless of outcome */
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
}
