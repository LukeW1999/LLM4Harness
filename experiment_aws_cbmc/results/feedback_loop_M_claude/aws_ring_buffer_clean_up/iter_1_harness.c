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
    /* 1. Declare and initialize a ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Initialize the ring buffer using aws_ring_buffer_init to get a valid structure */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Bound the size to prevent state space explosion */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    /* Initialize the ring buffer - only proceed if init succeeds */
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    /* Verify the ring buffer is valid before calling clean_up */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 3. Assert postconditions after clean_up */
    /* The function calls AWS_ZERO_STRUCT(*ring_buf), which zeros all fields */
    
    /* After AWS_ZERO_STRUCT, all fields should be zeroed/NULL */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);

    /* The atomic vars should also be zeroed - check via aws_ring_buffer_is_valid */
    /* After zeroing, the ring buffer should NOT be valid (allocation is NULL) */
    /* But we can check that the struct is fully zeroed */
    
    /* Verify that the allocation pointer is NULL (memory was released and struct zeroed) */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.allocator == NULL);
}
