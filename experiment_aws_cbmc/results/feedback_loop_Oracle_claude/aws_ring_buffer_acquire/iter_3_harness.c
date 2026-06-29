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
    /* requested_size must be > 0 and fit within the ring buffer */
    __CPROVER_assume(requested_size > 0);
    size_t ring_size = ring_buf.allocation_end - ring_buf.allocation;
    __CPROVER_assume(requested_size < ring_size);

    /* Save pre-call state for frame condition checks */
    uint8_t *allocation_pre = ring_buf.allocation;
    uint8_t *allocation_end_pre = ring_buf.allocation_end;
    struct aws_allocator *allocator_pre = ring_buf.allocator;

    /* Call the function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &buf);

    /* Postcondition 1: Return value correctness */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* Postcondition 2: If success, verify output buffer invariants */
    if (result == AWS_OP_SUCCESS) {
        /* The output buffer must be valid */
        assert(aws_byte_buf_is_valid(&buf));

        /* The output buffer capacity must equal requested_size */
        assert(buf.capacity == requested_size);

        /* The output buffer length must be 0 */
        assert(buf.len == 0);

        /* The output buffer pointer must be within the ring buffer allocation */
        assert(buf.buffer != NULL);
        assert(buf.buffer >= ring_buf.allocation);
        assert(buf.buffer + buf.capacity <= ring_buf.allocation_end);
    }

    /* Postcondition 4: Frame conditions - ring buffer allocation bounds unchanged */
    assert(ring_buf.allocation == allocation_pre);
    assert(ring_buf.allocation_end == allocation_end_pre);
    assert(ring_buf.allocator == allocator_pre);

    /* Postcondition 5: Ring buffer remains valid regardless of outcome */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
