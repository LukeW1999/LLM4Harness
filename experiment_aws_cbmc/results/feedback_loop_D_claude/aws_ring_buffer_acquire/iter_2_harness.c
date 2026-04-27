#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 64

void aws_ring_buffer_acquire_harness(void) {
    /* Set up ring buffer */
    struct aws_ring_buffer ring_buf;
    
    /* We need a valid ring buffer with a real allocation */
    size_t buf_size;
    __CPROVER_assume(buf_size > 0 && buf_size <= MAX_BUFFER_SIZE);
    
    ring_buf.allocator = aws_default_allocator();
    ring_buf.allocation = malloc(buf_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + buf_size;
    
    /* Set head and tail to valid pointers within the allocation */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= buf_size);
    __CPROVER_assume(tail_offset <= buf_size);
    
    uint8_t *head_ptr = ring_buf.allocation + head_offset;
    uint8_t *tail_ptr = ring_buf.allocation + tail_offset;
    
    aws_atomic_store_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_store_ptr(&ring_buf.tail, (void *)tail_ptr);
    
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    
    /* Set up requested_size */
    size_t requested_size;
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= buf_size);
    
    /* Set up dest */
    struct aws_byte_buf dest;
    
    /* Save old state */
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    struct aws_allocator *old_allocator = ring_buf.allocator;
    
    /* Call the function */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);
    
    /* Frame conditions: these never change */
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);
    assert(ring_buf.allocator == old_allocator);
    
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
    
    if (result == AWS_OP_SUCCESS) {
        /* dest must be valid */
        assert(dest.buffer != NULL);
        assert(dest.len == 0);
        assert(dest.capacity == requested_size);
        assert(dest.allocator == NULL);
        
        /* dest->buffer must be within ring_buf allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        
        /* head must have changed to point past the acquired region */
        assert(new_head == dest.buffer + requested_size);
        
        /* head must be within valid range */
        assert(new_head >= ring_buf.allocation);
        assert(new_head <= ring_buf.allocation_end);
        
        /* ring buffer must still be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));
        
    } else {
        /* On failure, head and tail must be unchanged */
        assert(new_head == old_head);
        assert(new_tail == old_tail);
        
        /* ring buffer must still be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }
}
