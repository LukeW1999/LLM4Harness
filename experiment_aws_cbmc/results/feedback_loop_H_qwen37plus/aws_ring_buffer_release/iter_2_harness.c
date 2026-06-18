#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_release_harness() {
    struct aws_ring_buffer ring_buffer;
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    /* Ensure buf->buffer is within ring_buffer's allocation */
    __CPROVER_assume(buf.buffer >= ring_buffer.allocation);
    __CPROVER_assume(buf.buffer + buf.capacity <= ring_buffer.allocation_end);
    
    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;
    
    /* Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);
    
    /* Assert changed fields */
    assert(aws_atomic_load_ptr(&ring_buffer.tail) == (void *)(old_buf.buffer + old_buf.capacity));
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    
    /* Assert unchanged fields */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert(aws_atomic_load_ptr(&ring_buffer.head) == aws_atomic_load_ptr(&old_ring_buffer.head));
    
    /* Assert validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
}
