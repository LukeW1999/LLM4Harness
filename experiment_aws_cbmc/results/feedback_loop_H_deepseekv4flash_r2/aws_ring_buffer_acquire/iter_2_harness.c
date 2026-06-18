#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_ring_buffer_acquire_harness() {
    struct aws_ring_buffer ring_buf;
    size_t requested_size;
    struct aws_byte_buf dest;

    /* Initialize dest to a well-defined state */
    dest.allocator = NULL;
    dest.buffer = NULL;
    dest.capacity = 0;
    dest.len = 0;

    /* Initialize the ring buffer with a nondeterministic capacity */
    size_t capacity;
    __CPROVER_assume(capacity >= 1);
    int init_result = aws_ring_buffer_init(&ring_buf, aws_default_allocator(), capacity);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    /* Assume requested_size is positive and fits within the buffer */
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= capacity);

    /* Save old state for failure case (though we expect success) */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* Call the function */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer != NULL);
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
        assert(dest.allocator == NULL);  /* ring buffer does not set allocator */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* On failure, dest should be unchanged */
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.len == old_dest.len);
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }

    /* Unchanged fields (regardless of result) */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    /* Validity invariant */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
