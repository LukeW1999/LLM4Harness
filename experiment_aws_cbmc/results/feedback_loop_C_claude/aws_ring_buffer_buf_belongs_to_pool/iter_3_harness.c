#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* Set up ring buffer */
    struct aws_ring_buffer ring_buffer;
    
    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
    
    ring_buffer.allocator = aws_default_allocator();
    ring_buffer.allocation = malloc(size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + size;
    
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= size);
    __CPROVER_assume(tail_offset <= size);
    aws_atomic_init_ptr(&ring_buffer.head, (void *)(ring_buffer.allocation + head_offset));
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)(ring_buffer.allocation + tail_offset));
    
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    
    /* Set up byte buf with nondet buffer pointer to cover both branches */
    struct aws_byte_buf buf;
    buf.allocator = aws_default_allocator();
    buf.len = 0;
    buf.capacity = 0;
    
    /* Use nondet to decide if buf.buffer points inside or outside the ring buffer */
    uint8_t *nondet_ptr;
    buf.buffer = nondet_ptr;
    
    /* Make sure buf is valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;
    
    /* Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);
    
    /* === FRAME CONDITIONS: ring_buffer fields are UNCHANGED === */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    
    /* === FRAME CONDITIONS: buf fields are UNCHANGED === */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    
    /* === VALIDITY INVARIANTS === */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
    
    /* === RESULT CORRECTNESS === */
    /* If result is true, buf->buffer must be within ring_buffer's allocation range */
    if (result) {
        assert(buf.buffer >= ring_buffer.allocation);
        assert(buf.buffer < ring_buffer.allocation_end);
    }
    
    /* If buf->buffer is within range, result must be true */
    if (buf.buffer != NULL && 
        buf.buffer >= ring_buffer.allocation && 
        buf.buffer < ring_buffer.allocation_end) {
        assert(result);
    }
}
