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
    
    /* Allocate the ring buffer's internal allocation */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    
    ring_buffer.allocator = aws_default_allocator();
    ring_buffer.allocation = malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;
    
    /* Set up atomic head and tail to valid positions within the buffer */
    uint8_t *head_ptr;
    uint8_t *tail_ptr;
    
    /* head and tail must be within [allocation, allocation_end] */
    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= ring_size);
    __CPROVER_assume(tail_offset <= ring_size);
    
    head_ptr = ring_buffer.allocation + head_offset;
    tail_ptr = ring_buffer.allocation + tail_offset;
    
    aws_atomic_init_ptr(&ring_buffer.head, (void *)head_ptr);
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)tail_ptr);
    
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    
    /* 2. Set up the byte buffer to release */
    struct aws_byte_buf buf;
    
    /* buf.buffer must point within the ring buffer's allocation */
    size_t buf_offset = nondet_size_t();
    size_t buf_capacity = nondet_size_t();
    
    __CPROVER_assume(buf_offset <= ring_size);
    __CPROVER_assume(buf_capacity <= ring_size - buf_offset);
    __CPROVER_assume(buf_capacity > 0);
    
    buf.buffer = ring_buffer.allocation + buf_offset;
    buf.capacity = buf_capacity;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = ring_buffer.allocator;
    
    /* buf.buffer + buf.capacity must be within [allocation, allocation_end] */
    __CPROVER_assume(buf.buffer + buf.capacity <= ring_buffer.allocation_end);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    /* 3. Save old state */
    struct aws_ring_buffer old_ring = ring_buffer;
    
    /* 4. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);
    
    /* 5. Assert postconditions */
    
    /* The function stores buf->buffer + buf->capacity into the tail atomic */
    uint8_t *expected_tail = old_ring.allocation + buf_offset + buf_capacity;
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(new_tail == expected_tail);
    
    /* buf is zeroed out after release (AWS_ZERO_STRUCT(*buf)) */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);
    
    /* Ring buffer fields other than tail should be unchanged */
    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);
    
    /* Head should be unchanged */
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&old_ring.head);
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    assert(new_head == old_head);
    
    /* Ring buffer validity: new tail must be within bounds */
    assert(new_tail >= ring_buffer.allocation);
    assert(new_tail <= ring_buffer.allocation_end);
}
