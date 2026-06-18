#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness() {
    /* 1. Set up ring buffer */
    struct aws_ring_buffer ring_buffer;
    
    /* Initialize ring buffer with a valid allocation */
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
    
    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Set up buf - must belong to the ring buffer */
    struct aws_byte_buf buf;
    
    /* buf.buffer must point within the ring buffer's allocation */
    size_t offset;
    __CPROVER_assume(offset < size);
    buf.buffer = ring_buffer.allocation + offset;
    
    /* buf.capacity must be such that buf.buffer + buf.capacity <= allocation_end */
    size_t capacity;
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(buf.buffer + capacity <= ring_buffer.allocation_end);
    buf.capacity = capacity;
    
    /* Set len non-deterministically but valid */
    size_t len;
    __CPROVER_assume(len <= capacity);
    buf.len = len;
    
    buf.allocator = allocator;
    
    /* Verify buf belongs to pool */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));
    
    /* 3. Save old state */
    uint8_t *old_buf_buffer = buf.buffer;
    size_t old_buf_capacity = buf.capacity;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;
    struct aws_allocator *old_allocator = ring_buffer.allocator;
    
    /* 4. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);
    
    /* 5. Assert postconditions */
    
    /* After release, buf should be zeroed (AWS_ZERO_STRUCT(*buf)) */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);
    
    /* Ring buffer's allocation and allocator should be unchanged */
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);
    assert(ring_buffer.allocator == old_allocator);
    
    /* Ring buffer validity should still hold */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    
    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buffer);
}
