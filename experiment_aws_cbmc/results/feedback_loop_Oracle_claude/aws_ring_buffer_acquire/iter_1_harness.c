#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness(void) {
    /* Setup */
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;
    size_t requested_size;

    /* Ground-truth preconditions */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save pre-call state for frame condition checks */
    struct aws_ring_buffer ring_buf_pre = ring_buf;

    /* Call the function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &buf);

    /* Postcondition 1: Return value correctness */
    /* requested_size must be non-zero (precondition), result is either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* Postcondition 2: If success, verify output buffer invariants */
    if (result == AWS_OP_SUCCESS) {
        /* The output buffer must be valid */
        assert(aws_byte_buf_is_valid(&buf));

        /* The output buffer capacity must equal requested_size */
        assert(buf.capacity == requested_size);

        /* The output buffer length must be 0 (from_empty_array sets len=0) */
        assert(buf.len == 0);

        /* The output buffer pointer must be within the ring buffer allocation */
        assert(buf.buffer != NULL);
        assert(buf.buffer >= ring_buf.allocation);
        assert(buf.buffer + buf.capacity <= ring_buf.allocation_end);

        /* The ring buffer must still be valid after a successful acquire */
        assert(aws_ring_buffer_is_valid(&ring_buf));

        /* The allocator in the output buffer should match the ring buffer's allocator */
        assert(buf.allocator == ring_buf.allocator);
    }

    /* Postcondition 3: If error, verify error code is set */
    if (result == AWS_OP_ERR) {
        assert(aws_last_error() == AWS_ERROR_OOM);
    }

    /* Postcondition 4: Frame conditions - ring buffer allocation bounds unchanged */
    assert(ring_buf.allocation == ring_buf_pre.allocation);
    assert(ring_buf.allocation_end == ring_buf_pre.allocation_end);
    assert(ring_buf.allocator == ring_buf_pre.allocator);

    /* Postcondition 5: Ring buffer remains valid regardless of outcome */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}

void aws_ring_buffer_acquire_harness(void) {
    aws_ring_buffer_acquire_harness();
    return 0;
}
