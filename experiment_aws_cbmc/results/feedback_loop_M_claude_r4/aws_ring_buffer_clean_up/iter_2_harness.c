#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_ring_buffer_clean_up_harness() {
    /* 1. Declare and initialize a ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Initialize the ring buffer with a valid allocator and bounded size */
    struct aws_allocator *allocator = aws_default_allocator();
    
    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
    
    /* Initialize the ring buffer - this sets up allocation, head, tail, allocation_end */
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    
    /* Only proceed if initialization succeeded */
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 3. Assert postconditions after clean_up:
     * The function calls AWS_ZERO_STRUCT(*ring_buf) which zeros all fields.
     * After clean_up:
     * - ring_buf.allocator should be NULL (zeroed)
     * - ring_buf.allocation should be NULL (zeroed)
     * - ring_buf.allocation_end should be NULL (zeroed)
     */
    
    /* All pointer fields should be NULL after zeroing */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
}
