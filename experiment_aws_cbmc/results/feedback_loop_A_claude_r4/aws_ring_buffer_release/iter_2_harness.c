#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

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
    
    /* Set head and tail atomics to valid positions within the buffer */
    uint8_t *head_ptr;
    uint8_t *tail_ptr;
    __CPROVER_assume((uintptr_t)head_ptr >= (uintptr_t)ring_buffer.allocation);
    __CPROVER_assume((uintptr_t)head_ptr <= (uintptr_t)ring_buffer.allocation_end);
    __CPROVER_assume((uintptr_t)tail_ptr >= (uintptr_t)ring_buffer.allocation);
    __CPROVER_assume((uintptr_t)tail_ptr <= (uintptr_t)ring_buffer.allocation_end);
    
    aws_atomic_store_ptr(&ring_buffer.head, (void *)head_ptr);
    aws_atomic_store_ptr(&ring_buffer.tail, (void *)tail_ptr);
    
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    
    /* 2. Set up buf that belongs to the ring buffer */
    struct aws_byte_buf buf;
    
    /* buf.buffer must point within the ring buffer's allocation */
    uint8_t *buf_start;
    size_t buf_capacity;
    __CPROVER_assume((uintptr_t)buf_start >= (uintptr_t)ring_buffer.allocation);
    __CPROVER_assume((uintptr_t)buf_start <= (uintptr_t)ring_buffer.allocation_end);
    __CPROVER_assume(buf_capacity <= (size_t)(ring_buffer.allocation_end - buf_start));
    __CPROVER_assume(buf_capacity > 0);
    
    buf.buffer = buf_start;
    buf.capacity = buf_capacity;
    size_t buf_len;
    __CPROVER_assume(buf_len <= buf_capacity);
    buf.len = buf_len;
    buf.allocator = ring_buffer.allocator;
    
    /* The new tail after release will be buf->buffer + buf->capacity */
    uint8_t *expected_new_tail = buf.buffer + buf.capacity;
    __CPROVER_assume((uintptr_t)expected_new_tail >= (uintptr_t)ring_buffer.allocation);
    __CPROVER_assume((uintptr_t)expected_new_tail <= (uintptr_t)ring_buffer.allocation_end);
    
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    /* 3. Save old ring buffer state */
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;
    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    
    /* 4. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);
    
    /* 5. Assert postconditions */
    
    /* The tail should now point to buf->buffer + buf->capacity (before zeroing) */
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(new_tail == expected_new_tail);
    
    /* buf should be zeroed out (AWS_ZERO_STRUCT) */
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
    
    /* Ring buffer should still be valid */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
