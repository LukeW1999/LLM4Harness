#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

void aws_ring_buffer_release_harness() {
    struct aws_ring_buffer ring_buffer;
    
    ring_buffer.allocator = aws_default_allocator();
    size_t capacity;
    __CPROVER_assume(capacity > 0);
    ring_buffer.allocation = malloc(capacity);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + capacity;
    
    aws_atomic_store_ptr(&ring_buffer.head, ring_buffer.allocation);
    aws_atomic_store_ptr(&ring_buffer.tail, ring_buffer.allocation);
    
    struct aws_byte_buf buf;
    buf.allocator = NULL;
    buf.len = 0;
    
    size_t buf_capacity;
    __CPROVER_assume(buf_capacity > 0 && buf_capacity <= capacity);
    buf.capacity = buf_capacity;
    
    size_t offset;
    __CPROVER_assume(offset + buf_capacity <= capacity);
    buf.buffer = ring_buffer.allocation + offset;
    
    aws_ring_buffer_release(&ring_buffer, &buf);
    
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
}
