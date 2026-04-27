#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* Set up ring buffer */
    struct aws_ring_buffer ring_buffer;
    
    /* We need a valid ring buffer - use nondet size and initialize */
    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
    
    /* Allocate the ring buffer's internal allocation manually for CBMC */
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buffer.allocator = allocator;
    ring_buffer.allocation = malloc(size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + size;
    
    /* Set head and tail to valid positions within the buffer */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= size);
    __CPROVER_assume(tail_offset <= size);
    aws_atomic_init_ptr(&ring_buffer.head, (void *)(ring_buffer.allocation + head_offset));
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)(ring_buffer.allocation + tail_offset));
    
    /* Verify ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    
    /* Set up byte buf */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;
    
    /* Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);
    
    /* === FRAME CONDITIONS: ring_buffer fields are UNCHANGED === */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    
    /* Check head and tail atomics are unchanged */
    uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    uint8_t *old_head_ptr = (uint8_t *)aws_atomic_load_ptr(&old_ring_buffer.head);
    assert(head_ptr == old_head_ptr);
    
    uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    uint8_t *old_tail_ptr = (uint8_t *)aws_atomic_load_ptr(&old_ring_buffer.tail);
    assert(tail_ptr == old_tail_ptr);
    
    /* === FRAME CONDITIONS: buf fields are UNCHANGED === */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    
    /* === VALIDITY INVARIANTS === */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
    
    /* === RESULT CORRECTNESS === */
    /* If result is true, buf->buffer must be within ring_buffer's allocation range */
    if (result) {
        assert(buf.buffer >= ring_buffer.allocation);
        assert(buf.buffer < ring_buffer.allocation_end);
    }
}
