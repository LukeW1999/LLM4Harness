#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    size_t requested_size = nondet_size_t();
    struct aws_byte_buf dest;

    /* Bound requested_size to avoid overflow in proof */
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    /* Initialize ring buffer */
    ring_buf.allocator = aws_default_allocator();
    __CPROVER_assume(ring_buf.allocator != NULL);

    /* Allocate the ring buffer's backing memory non-deterministically */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
    ring_buf.allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;

    /* Initialize atomic head and tail pointers to point within the allocation */
    uint8_t *head_ptr;
    uint8_t *tail_ptr;

    /* Non-deterministically choose head and tail positions */
    if (nondet_bool()) {
        /* Empty ring buffer case: head == tail */
        size_t offset = nondet_size_t();
        __CPROVER_assume(offset < alloc_size);
        head_ptr = ring_buf.allocation + offset;
        tail_ptr = head_ptr;
    } else {
        /* Non-empty: head and tail at different positions */
        size_t head_off = nondet_size_t();
        size_t tail_off = nondet_size_t();
        __CPROVER_assume(head_off < alloc_size);
        __CPROVER_assume(tail_off < alloc_size);
        /* Order can be head < tail or head > tail (wrap-around) */
        head_ptr = ring_buf.allocation + head_off;
        tail_ptr = ring_buf.allocation + tail_off;
    }

    aws_atomic_store_ptr(&ring_buf.head, head_ptr);
    aws_atomic_store_ptr(&ring_buf.tail, tail_ptr);

    /* Ensure the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    struct aws_allocator *old_allocator = ring_buf.allocator;

    /* Also save the old head and tail pointer values */
    uint8_t *old_head_ptr;
    uint8_t *old_tail_ptr;
    aws_atomic_load_ptr(&old_ring_buf.head, &old_head_ptr);
    aws_atomic_load_ptr(&old_ring_buf.tail, &old_tail_ptr);

    /* 3. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* On success, dest must be populated with a valid byte_buf */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest.buffer must point within ring_buf.allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.len <= ring_buf.allocation_end);

        /* dest.len must equal requested_size */
        assert(dest.len == requested_size);

        /* dest.capacity must equal requested_size */
        assert(dest.capacity == requested_size);

        /* The allocator in dest must match the ring buffer's allocator */
        assert(dest.allocator == ring_buf.allocator);

        /* Head pointer must have advanced by requested_size (or wrapped) */
        uint8_t *new_head_ptr;
        aws_atomic_load_ptr(&ring_buf.head, &new_head_ptr);

        /* If we didn't wrap, new head should be old head + requested_size */
        /* Or if wrapped, new head should be allocation + requested_size */
        if (old_head_ptr == old_tail_ptr) {
            /* Empty buffer case: head advanced from allocation base */
            assert(new_head_ptr == ring_buf.allocation + requested_size);
        } else if (old_tail_ptr > old_head_ptr) {
            /* head < tail case: allocate from head, no wrap */
            assert(new_head_ptr == old_head_ptr + requested_size);
        } else {
            /* tail < head case: may allocate from head or wrap */
            if (old_head_ptr + requested_size <= ring_buf.allocation_end) {
                assert(new_head_ptr == old_head_ptr + requested_size);
            } else {
                assert(new_head_ptr == ring_buf.allocation + requested_size);
            }
        }
    } else {
        /* On failure, the ring buffer state must remain unchanged */
        uint8_t *new_head_ptr;
        uint8_t *new_tail_ptr;
        aws_atomic_load_ptr(&ring_buf.head, &new_head_ptr);
        aws_atomic_load_ptr(&ring_buf.tail, &new_tail_ptr);

        assert(new_head_ptr == old_head_ptr);
        assert(new_tail_ptr == old_tail_ptr);

        /* dest should not be modified — its fields should remain as uninitialized
         * (we cannot assert uninitialized content, so we check ring buffer invariants only) */
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    /* Free allocated memory */
    free(ring_buf.allocation);
}
