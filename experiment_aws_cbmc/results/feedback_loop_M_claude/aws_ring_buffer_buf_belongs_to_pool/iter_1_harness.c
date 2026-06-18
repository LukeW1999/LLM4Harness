#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buf;
    
    /* Allocate a backing buffer for the ring buffer */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    
    /* Initialize ring buffer with a real allocator */
    int init_result = aws_ring_buffer_init(&ring_buf, aws_default_allocator(), ring_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Set up the byte buf to check */
    struct aws_byte_buf buf;
    
    /* Non-deterministically set the buffer pointer */
    /* It could point inside the ring buffer allocation or outside */
    uint8_t *buf_ptr = nondet_bool() ? 
        (ring_buf.allocation + (nondet_size_t() % ring_size)) : 
        (uint8_t *)nondet_size_t();
    
    buf.buffer = buf_ptr;
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = aws_default_allocator();

    /* 3. Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_buf = buf;

    /* 4. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* 5. Assert postconditions */
    
    /* The function returns true if buf->buffer is within [allocation, allocation_end) */
    if (result) {
        /* buf->buffer must be within the ring buffer's allocation range */
        assert(buf.buffer >= ring_buf.allocation);
        assert(buf.buffer < ring_buf.allocation_end);
    } else {
        /* buf->buffer is outside the ring buffer's allocation range */
        assert(buf.buffer < ring_buf.allocation || buf.buffer >= ring_buf.allocation_end);
    }

    /* 6. Assert unchanged fields — ring buffer should not be modified */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    /* 7. Assert buf is not modified */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 8. Assert validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
}
