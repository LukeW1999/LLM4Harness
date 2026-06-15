#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <aws/common/ring_buffer.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;

    /* nondeterministic size for the underlying allocation, bounded */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);

    /* allocate the backing memory for the ring buffer */
    ring_buf.allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL || alloc_size == 0);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;

    /* allocator pointer is not used by acquire, keep it nondeterministic */
    ring_buf.allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(ring_buf.allocator != NULL);

    /* initialise atomic head/tail pointers to some valid location inside the buffer */
    /* For simplicity we start with an empty buffer (head == tail) */
    AWS_ATOMIC_STORE_HEAD_PTR(&ring_buf, ring_buf.allocation);
    AWS_ATOMIC_STORE_TAIL_PTR(&ring_buf, ring_buf.allocation);

    /* assume the ring buffer satisfies its validity predicate */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* ensure dest has an allocated buffer member (its contents will be overwritten on success) */
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* 3. Choose a nondeterministic requested size (precondition: != 0) */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    /* 4. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* dest must point inside the ring buffer allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);

        /* capacity must equal the requested size (aws_byte_buf_from_empty_array) */
        assert(dest.capacity == requested_size);

        /* length is set to 0 for an empty buffer */
        assert(dest.len == 0);

        /* allocator, allocation base and allocation_end must remain unchanged */
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
    } else {
        /* On failure the ring buffer must be unchanged */
        assert(ring_buf.head == old_ring.head);
        assert(ring_buf.tail == old_ring.tail);
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);

        /* dest must be unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.len == old_dest.len);
    }

    /* 6. Fields that must NOT change regardless of result (re‑asserted for clarity) */
    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);

    /* 7. Validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
}
