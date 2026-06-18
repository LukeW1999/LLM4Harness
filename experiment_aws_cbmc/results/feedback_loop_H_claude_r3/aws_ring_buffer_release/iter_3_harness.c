#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness() {
    /* 1. Set up ring buffer */
    struct aws_ring_buffer ring_buffer;
    
    /* Initialize ring buffer with a valid allocation */
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;
    __CPROVER_assume(size > 0 && size <= 1024);
    
    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Set up buf - must belong to the ring buffer */
    struct aws_byte_buf buf;
    
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
    
    /* 3. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);
    
    /* 4. Assert postconditions */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);
    
    /* Ring buffer validity should still hold */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    
    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buffer);
}
