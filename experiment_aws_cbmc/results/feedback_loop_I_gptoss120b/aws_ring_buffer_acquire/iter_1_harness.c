#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer old_ring_buf;
    struct aws_byte_buf dest;
    struct aws_byte_buf old_dest;

    /* allocator */
    ring_buf.allocator = aws_default_allocator();

    /* non‑deterministic allocation size, bounded */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);

    ring_buf.allocation = malloc(alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;

    /* initialise atomic head and tail to the same pointer (empty buffer) */
    AWS_ATOMIC_STORE_HEAD_PTR(&ring_buf, ring_buf.allocation);
    AWS_ATOMIC_STORE_TAIL_PTR(&ring_buf, ring_buf.allocation);

    /* assume the ring buffer satisfies its cheap validity check */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* bound the destination byte buffer */
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* 2. Save old state BEFORE calling */
    old_ring_buf = ring_buf;
    old_dest = dest;

    /* 3. Non‑deterministic request size with precondition */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    /* 4. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Load head/tail after the call for assertions */
    uint8_t *new_head = NULL;
    uint8_t *new_tail = NULL;
    AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);

    uint8_t *old_head = NULL;
    uint8_t *old_tail = NULL;
    AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring_buf, old_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&old_ring_buf, old_tail);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* dest must point inside the ring buffer allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer < ring_buf.allocation_end);
        /* length and capacity must equal the requested size */
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);
        /* allocator and allocation bounds must be unchanged */
        assert(ring_buf.allocator == old_ring_buf.allocator);
        assert(ring_buf.allocation == old_ring_buf.allocation);
        assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
        /* tail must remain unchanged */
        assert(new_tail == old_tail);
    } else {
        /* on failure the ring buffer must be unchanged */
        assert(new_head == old_head);
        assert(new_tail == old_tail);
        assert(ring_buf.allocator == old_ring_buf.allocator);
        assert(ring_buf.allocation == old_ring_buf.allocation);
        assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
        /* dest must be unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
    }

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
}
