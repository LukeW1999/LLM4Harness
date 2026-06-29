#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* Allocate and set up ring buffer */
    struct aws_ring_buffer ring_buf;
    size_t size;
    __CPROVER_assume(size > 0 && size <= 1024);
    
    /* Initialize ring buffer with a valid allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    
    /* Set up head and tail to make ring buffer non-empty */
    /* We need head != tail for non-empty */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset < size);
    __CPROVER_assume(tail_offset < size);
    __CPROVER_assume(head_offset != tail_offset);
    
    aws_atomic_store_ptr(&ring_buf.head, (void *)(ring_buf.allocation + head_offset));
    aws_atomic_store_ptr(&ring_buf.tail, (void *)(ring_buf.allocation + tail_offset));
    
    /* Set up byte buf */
    struct aws_byte_buf buf;
    uint8_t *buf_ptr;
    size_t buf_len;
    size_t buf_cap;
    
    /* Make buf valid - either inside or outside the ring buffer */
    bool inside;
    if (inside && size > 0) {
        size_t offset;
        __CPROVER_assume(offset < size);
        __CPROVER_assume(buf_cap <= size - offset);
        __CPROVER_assume(buf_len <= buf_cap);
        buf.buffer = ring_buf.allocation + offset;
        buf.len = buf_len;
        buf.capacity = buf_cap;
        buf.allocator = allocator;
    } else {
        /* Buffer outside ring buffer */
        uint8_t *external_buf = malloc(64);
        __CPROVER_assume(external_buf != NULL);
        __CPROVER_assume(buf_cap <= 64);
        __CPROVER_assume(buf_len <= buf_cap);
        buf.buffer = external_buf;
        buf.len = buf_len;
        buf.capacity = buf_cap;
        buf.allocator = allocator;
    }
    
    /* Apply preconditions */
    __CPROVER_assume(!aws_ring_buffer_is_empty(&ring_buf));
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    /* Save state before call for frame condition checks */
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    size_t old_buf_len = buf.len;
    size_t old_buf_cap = buf.capacity;
    uint8_t *old_buf_ptr = buf.buffer;
    
    /* Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);
    
    /* Postcondition 1: Return value correctness
     * If buf->buffer is within [allocation, allocation_end), result should be true */
    if (buf.buffer != NULL && 
        buf.buffer >= ring_buf.allocation && 
        buf.buffer < ring_buf.allocation_end) {
        assert(result == true);
    }
    
    /* If buf->buffer is outside [allocation, allocation_end), result should be false */
    if (buf.buffer != NULL && 
        (buf.buffer < ring_buf.allocation || buf.buffer >= ring_buf.allocation_end)) {
        assert(result == false);
    }
    
    /* Postcondition 2: Ring buffer validity is preserved */
    assert(aws_ring_buffer_is_valid(&ring_buf));
    
    /* Postcondition 3: Frame conditions - ring buffer allocation not modified */
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);
    
    /* Postcondition 4: Input buf not modified */
    assert(buf.len == old_buf_len);
    assert(buf.capacity == old_buf_cap);
    assert(buf.buffer == old_buf_ptr);
    
    /* Postcondition 5: buf remains valid after the call */
    assert(aws_byte_buf_is_valid(&buf));
    
    /* Postcondition 6: Result is a valid boolean */
    assert(result == true || result == false);
    
    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
}
