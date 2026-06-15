#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* Declare and initialize ring_buffer */
    struct aws_ring_buffer ring_buffer;
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    
    /* Declare and initialize byte_buf */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;
    
    /* Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);
    
    /* Assert unchanged fields: ring_buffer */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    /* Note: head and tail are atomic, but pointer equality should hold */
    assert(ring_buffer.head == old_ring_buffer.head);
    assert(ring_buffer.tail == old_ring_buffer.tail);
    
    /* Assert unchanged fields: buf */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.len == old_buf.len);
    assert(buf.allocator == old_buf.allocator);
    
    /* Assert validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
}
