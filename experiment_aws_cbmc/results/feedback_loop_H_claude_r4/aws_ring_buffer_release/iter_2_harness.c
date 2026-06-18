#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness() {
    /* 1. Set up ring buffer */
    struct aws_ring_buffer ring_buffer;
    
    /* Allocate the ring buffer's internal allocation */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buffer.allocator = allocator;
    ring_buffer.allocation = malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;
    
    /* Set up atomic head and tail to valid positions within the buffer */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= ring_size);
    __CPROVER_assume(tail_offset <= ring_size);
    
    uint8_t *head_ptr = ring_buffer.allocation + head_offset;
    uint8_t *tail_ptr = ring_buffer.allocation + tail_offset;
    
    aws_atomic_init_ptr(&ring_buffer.head, (void *)head_ptr);
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)tail_ptr);
    
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    
    /* 2. Set up the byte buffer to release */
    struct aws_byte_buf buf;
    
    /* buf.buffer must point within the ring buffer's allocation */
    size_t buf_offset;
    size_t buf_capacity;
    
    __CPROVER_assume(buf_offset <= ring_size);
    __CPROVER_assume(buf_capacity <= ring_size - buf_offset);
    __CPROVER_assume(buf_capacity > 0);
    
    buf.buffer = ring_buffer.allocation + buf_offset;
    buf.capacity = buf_capacity;
    size_t buf_len;
    __CPROVER_assume(buf_len <= buf_capacity);
    buf.len = buf_len;
    buf.allocator = ring_buffer.allocator;
    
    /* buf.buffer + buf.capacity must be within [allocation, allocation_end] */
    __CPROVER_assume(buf.buffer + buf.capacity <= ring_buffer.allocation_end);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    /* 3. Save old state */
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;
    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    uint8_t *expected_tail = ring_buffer.allocation + buf_offset + buf_capacity;
    
    /* 4. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);
    
    /* 5. Assert postconditions */
    
    /* The function stores buf->buffer + buf->capacity into the tail atomic */
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(new_tail == expected_tail);
    
    /* buf is zeroed out after release (AWS_ZERO_STRUCT(*buf)) */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);
    
    /* Ring buffer fields other than tail should be unchanged */
    assert(ring_buffer.allocator == old_allocator);
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);
    
    /* Head should be unchanged */
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    assert(new_head == old_head);
    
    /* Ring buffer validity: new tail must be within bounds */
    assert(new_tail >= ring_buffer.allocation);
    assert(new_tail <= ring_buffer.allocation_end);
}
