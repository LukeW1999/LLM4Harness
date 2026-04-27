#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* Allocate and set up ring_buffer */
    struct aws_ring_buffer ring_buffer;
    
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buffer.allocator = allocator;
    
    /* Create a buffer allocation of some size */
    size_t buf_size;
    __CPROVER_assume(buf_size > 0 && buf_size <= MAX_BUFFER_SIZE);
    ring_buffer.allocation = malloc(buf_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + buf_size;
    
    /* Set head and tail to valid pointers within the allocation */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= buf_size);
    __CPROVER_assume(tail_offset <= buf_size);
    
    aws_atomic_init_ptr(&ring_buffer.head, (void *)(ring_buffer.allocation + head_offset));
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)(ring_buffer.allocation + tail_offset));
    
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    
    /* Set up byte_buf - must satisfy aws_byte_buf_is_valid */
    struct aws_byte_buf buf;
    
    /* Make buf valid using the helper or manually */
    size_t buf_capacity;
    size_t buf_len;
    __CPROVER_assume(buf_capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf_len <= buf_capacity);
    
    uint8_t *buf_ptr;
    if (buf_capacity > 0) {
        /* buf_ptr must be non-null if capacity > 0 for aws_byte_buf_is_valid */
        __CPROVER_assume(buf_ptr != NULL);
    } else {
        /* capacity == 0: buffer must be NULL, len must be 0 */
        buf_ptr = NULL;
        buf_len = 0;
    }
    
    buf.buffer = buf_ptr;
    buf.len = buf_len;
    buf.capacity = buf_capacity;
    buf.allocator = allocator;
    
    /* Ensure buf is valid before calling the function */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    /* Save old state */
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;
    struct aws_allocator *old_allocator = ring_buffer.allocator;
    
    uint8_t *old_buf_buffer = buf.buffer;
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;
    struct aws_allocator *old_buf_allocator = buf.allocator;
    
    /* Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);
    
    /* === ASSERTIONS: Frame conditions - nothing should change === */
    assert(ring_buffer.allocator == old_allocator);
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);
    
    assert(buf.buffer == old_buf_buffer);
    assert(buf.len == old_buf_len);
    assert(buf.capacity == old_buf_capacity);
    assert(buf.allocator == old_buf_allocator);
    
    /* === ASSERTIONS: Correctness of result === */
    /* If result is true, then buf.buffer must be within [allocation, allocation_end) */
    if (result) {
        assert(buf.buffer >= ring_buffer.allocation);
        assert(buf.buffer <= ring_buffer.allocation_end);
    }
    
    /* Result is a valid bool */
    assert(result == true || result == false);
    
    /* Ring buffer validity preserved */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
