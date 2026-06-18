#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_ring_buffer_clean_up_harness() {
    /* 1. Set up a valid ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Initialize with a real allocator and bounded size */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Initialize the ring buffer properly */
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 3. Assert postconditions:
     * After clean_up, AWS_ZERO_STRUCT zeroes out the entire structure.
     * All fields should be zero/NULL after cleanup.
     */

    /* The allocation pointer should be NULL after cleanup */
    assert(ring_buf.allocation == NULL);

    /* The allocator pointer should be NULL after cleanup */
    assert(ring_buf.allocator == NULL);

    /* The allocation_end pointer should be NULL after cleanup */
    assert(ring_buf.allocation_end == NULL);

    /* Head and tail atomics should be zeroed out.
     * We check that the ring buffer is no longer valid (since allocation is NULL)
     * or that the zeroed struct reflects cleanup.
     */

    /* After AWS_ZERO_STRUCT, the ring buffer should not be valid
     * (allocation is NULL, so aws_ring_buffer_is_valid should return false) */
    assert(!aws_ring_buffer_is_valid(&ring_buf));
}
