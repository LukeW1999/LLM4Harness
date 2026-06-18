#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_ring_buffer_release_harness() {
    /* 1. Set up ring buffer */
    struct aws_ring_buffer ring_buffer;
    
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;
    __CPROVER_assume(size > 0 && size <= 256);
    
    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Initialize buf to a valid zero state before passing to acquire */
    struct aws_byte_buf buf;
    buf.buffer = NULL;
    buf.len = 0;
    buf.capacity = 0;
    buf.allocator = NULL;
    
    size_t requested_size;
    __CPROVER_assume(requested_size > 0 && requested_size <= size);
    
    int acquire_result = aws_ring_buffer_acquire(&ring_buffer, requested_size, &buf);
    
    /* Only proceed if acquire succeeded */
    __CPROVER_assume(acquire_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));
    
    /* 3. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);
    
    /* 4. Assert postconditions */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);
    
    /* Ring buffer validity should still hold */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    
    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buffer);
}
