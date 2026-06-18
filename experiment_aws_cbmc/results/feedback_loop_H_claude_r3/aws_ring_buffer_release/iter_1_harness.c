#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

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
    /* and buf.buffer + buf.capacity must be within allocation_end */
    size_t offset;
    size_t capacity;
    __CPROVER_assume(offset < size);
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(offset + capacity <= size);
    
    buf.buffer = ring_buffer.allocation + offset;
    buf.capacity = capacity;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = allocator;
    
    /* Ensure buf belongs to the ring buffer */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));
    
    /* Save old ring buffer state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    
    /* Save old buf state */
    uint8_t *old_buf_buffer = buf.buffer;
    size_t old_buf_capacity = buf.capacity;
    
    /* 3. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);
    
    /* 4. Assert postconditions */
    
    /* After release, buf should be zeroed (AWS_ZERO_STRUCT(*buf)) */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);
    
    /* Ring buffer's tail should have been updated to buf->buffer + buf->capacity */
    /* (the tail atomic was set to old_buf_buffer + old_buf_capacity) */
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(new_tail == old_buf_buffer + old_buf_capacity);
    
    /* Ring buffer allocation and allocator should be unchanged */
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    
    /* Head should be unchanged */
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&old_ring_buffer.head);
    assert(new_head == old_head);
    
    /* Ring buffer validity should still hold */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    
    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buffer);
}
