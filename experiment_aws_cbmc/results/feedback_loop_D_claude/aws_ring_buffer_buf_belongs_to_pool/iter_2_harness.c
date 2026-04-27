#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

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
    
    /* Allocate and set up byte_buf */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;
    
    /* Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);
    
    /* === ASSERTIONS: Frame conditions - nothing should change === */
    
    /* ring_buffer fields unchanged */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    
    /* buf fields unchanged */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    
    /* === ASSERTIONS: Validity invariants === */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
    
    /* Result is a valid bool */
    assert(result == true || result == false);
}
