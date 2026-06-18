#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness() {
    /* 1. Declare and set up the ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Initialize with a valid ring buffer using aws_ring_buffer_init */
    struct aws_allocator *allocator = aws_default_allocator();

    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    /* Initialize the ring buffer */
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* Only proceed if initialization succeeded */
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 3. Assert postconditions after clean_up */
    /* The function calls AWS_ZERO_STRUCT(*ring_buf), so all fields should be zeroed */
    
    /* allocator should be NULL after zeroing */
    assert(ring_buf.allocator == NULL);
    
    /* allocation pointer should be NULL after zeroing */
    assert(ring_buf.allocation == NULL);
    
    /* allocation_end pointer should be NULL after zeroing */
    assert(ring_buf.allocation_end == NULL);
    
    /* head atomic should be zeroed - check the value stored */
    /* After AWS_ZERO_STRUCT, the entire struct is zeroed out */
    /* We verify the struct is zeroed by checking key pointer fields */
    
    /* The ring buffer should no longer be valid (allocation is NULL) */
    /* aws_ring_buffer_is_valid checks that allocation is non-NULL, so after cleanup it should be invalid */
    assert(!aws_ring_buffer_is_valid(&ring_buf));
}
