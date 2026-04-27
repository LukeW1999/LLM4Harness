#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 64

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;
    
    size_t buf_size;
    __CPROVER_assume(buf_size > 0 && buf_size <= MAX_BUFFER_SIZE);
    
    uint8_t *allocation = malloc(buf_size);
    __CPROVER_assume(allocation != NULL);
    
    ring_buf.allocator = aws_default_allocator();
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + buf_size;
    
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= buf_size);
    __CPROVER_assume(tail_offset <= buf_size);
    
    aws_atomic_store_ptr(&ring_buf.head, (void *)(allocation + head_offset));
    aws_atomic_store_ptr(&ring_buf.tail, (void *)(allocation + tail_offset));
    
    /* Don't use aws_ring_buffer_is_valid as a precondition - it may be too restrictive */
    
    size_t requested_size;
    __CPROVER_assume(requested_size > 0 && requested_size <= MAX_BUFFER_SIZE);
    
    struct aws_byte_buf dest;
    /* Initialize dest to known state */
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;
    dest.allocator = NULL;
    
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    struct aws_allocator *old_allocator = ring_buf.allocator;
    
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);
    
    /* Frame conditions */
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);
    assert(ring_buf.allocator == old_allocator);
    
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
    
    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer != NULL);
        assert(dest.len == 0);
        assert(dest.capacity == requested_size);
        assert(dest.allocator == NULL);
        
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        
        /* tail must be unchanged on success */
        assert(new_tail == old_tail);
    } else {
        /* On failure, head and tail must be unchanged */
        assert(new_head == old_head);
        assert(new_tail == old_tail);
    }
}
