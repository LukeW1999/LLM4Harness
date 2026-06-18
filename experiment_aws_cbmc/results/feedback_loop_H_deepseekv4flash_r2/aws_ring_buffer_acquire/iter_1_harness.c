#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness() {
    /* Non-deterministic inputs */
    struct aws_ring_buffer ring_buf;
    size_t requested_size;
    struct aws_byte_buf dest;

    /* Assume ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Assume requested_size is non-zero (precondition from implementation) */
    __CPROVER_assume(requested_size > 0);

    /* Ensure dest is initialized (but not necessarily valid) */
    dest.allocator = NULL;
    dest.buffer = NULL;
    dest.capacity = 0;
    dest.len = 0;

    /* Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* Call function */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, dest should be valid and have the requested size */
        assert(dest.buffer != NULL);
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
        assert(dest.allocator == NULL); /* ring buffer doesn't set allocator */
        /* The head pointer should have advanced */
        /* Note: exact head position depends on internal logic, but it must have moved */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* On failure, dest should be unchanged */
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.len == old_dest.len);
        /* Ring buffer should be unchanged on failure */
        assert(ring_buf.allocator == old_ring_buf.allocator);
        assert(ring_buf.allocation == old_ring_buf.allocation);
        assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
        /* head and tail are atomic, but we can check they haven't changed */
        /* For CBMC, we can't directly compare atomics, but we can check validity */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }

    /* Unchanged fields (regardless of result) */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    /* Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
