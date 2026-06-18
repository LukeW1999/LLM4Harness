#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buf;
    
    /* Allocate a backing buffer for the ring buffer */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    
    struct aws_allocator *allocator = aws_default_allocator();
    
    /* Initialize ring buffer with a real allocator */
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, ring_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Set up the byte buf to test */
    struct aws_byte_buf buf;
    buf.allocator = allocator;
    buf.len = 0;
    buf.capacity = 0;
    
    /* Non-deterministically choose whether buf.buffer points inside or outside the ring buffer */
    int buf_inside;
    
    if (buf_inside) {
        /* Make buf.buffer point somewhere within the ring buffer's allocation */
        size_t offset;
        __CPROVER_assume(offset < ring_size);
        buf.buffer = ring_buf.allocation + offset;
    } else {
        /* Make buf.buffer point somewhere outside or be NULL */
        uint8_t *nondet_ptr;
        buf.buffer = nondet_ptr;
        /* Ensure it doesn't accidentally point inside the ring buffer */
        __CPROVER_assume(buf.buffer < ring_buf.allocation || buf.buffer >= ring_buf.allocation_end);
    }

    /* 3. Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_buf = buf;

    /* 4. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* 5. Assert postconditions */
    
    /* The function returns true if buf.buffer is within the ring buffer's allocation range */
    if (result) {
        /* buf.buffer must be within [allocation, allocation_end) */
        assert(buf.buffer >= ring_buf.allocation);
        assert(buf.buffer < ring_buf.allocation_end);
    }
    
    /* The function should not modify the ring buffer */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
    
    /* The function should not modify the buf */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    
    /* Ring buffer validity must still hold */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
}
