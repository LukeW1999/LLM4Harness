#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buf;
    
    /* Allocate a backing buffer for the ring buffer */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    
    /* Initialize ring buffer with a real allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, ring_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Set up the byte buf to check */
    struct aws_byte_buf buf;
    
    size_t offset;
    uint8_t *nondet_ptr;
    
    bool use_inside;
    
    if (use_inside) {
        __CPROVER_assume(offset < ring_size);
        buf.buffer = ring_buf.allocation + offset;
    } else {
        buf.buffer = nondet_ptr;
    }
    
    size_t buf_len;
    size_t buf_capacity;
    __CPROVER_assume(buf_len <= buf_capacity);
    buf.len = buf_len;
    buf.capacity = buf_capacity;
    buf.allocator = allocator;

    /* 3. Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_buf = buf;

    /* 4. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* 5. Assert postconditions */
    if (result) {
        assert(buf.buffer >= ring_buf.allocation);
        assert(buf.buffer < ring_buf.allocation_end);
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
