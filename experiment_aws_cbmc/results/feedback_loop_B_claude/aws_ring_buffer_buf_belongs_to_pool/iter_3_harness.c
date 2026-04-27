#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Set up ring_buffer with a concrete allocation */
    struct aws_ring_buffer ring_buffer;
    
    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);
    
    uint8_t *allocation = malloc(alloc_size);
    __CPROVER_assume(allocation != NULL);
    
    ring_buffer.allocator = aws_default_allocator();
    ring_buffer.allocation = allocation;
    ring_buffer.allocation_end = allocation + alloc_size;
    
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= alloc_size);
    __CPROVER_assume(tail_offset <= alloc_size);
    aws_atomic_init_ptr(&ring_buffer.head, (void *)(allocation + head_offset));
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)(allocation + tail_offset));
    
    /* 2. Set up byte_buf with a non-deterministic buffer pointer */
    struct aws_byte_buf buf;
    buf.allocator = aws_default_allocator();
    buf.len = 0;
    buf.capacity = 0;
    
    /* Non-deterministically choose whether buf.buffer points inside or outside */
    uint8_t *buf_ptr;
    buf.buffer = buf_ptr;
    
    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);
    
    /* 4. Assert postconditions based on what the function should return */
    /* The function checks: buf->buffer >= allocation && buf->buffer < allocation_end */
    
    if (buf.buffer != NULL && 
        buf.buffer >= ring_buffer.allocation && 
        buf.buffer < ring_buffer.allocation_end) {
        /* buf is within the pool, result should be true */
        assert(result == true);
    } else {
        /* buf is outside the pool, result should be false */
        assert(result == false);
    }
    
    /* Also verify the converse: if result is true, buffer must be in range */
    if (result) {
        assert(buf.buffer != NULL);
        assert(buf.buffer >= ring_buffer.allocation);
        assert(buf.buffer < ring_buffer.allocation_end);
    }
    
    if (!result) {
        assert(buf.buffer == NULL || 
               buf.buffer < ring_buffer.allocation || 
               buf.buffer >= ring_buffer.allocation_end);
    }
    
    /* 5. Assert neither structure was modified */
    assert(ring_buffer.allocation == allocation);
    assert(ring_buffer.allocation_end == allocation + alloc_size);
    assert(buf.buffer == buf_ptr);
}
