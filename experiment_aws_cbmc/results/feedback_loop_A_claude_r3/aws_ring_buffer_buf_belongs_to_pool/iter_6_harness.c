#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 1024
#endif

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* Set up ring buffer */
    struct aws_ring_buffer ring_buf;

    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Set up byte buf - must be aws_byte_buf_is_valid */
    struct aws_byte_buf buf;
    
    /* Make a valid byte buf: either buffer is NULL with len=0 and capacity=0,
       or buffer is non-NULL with valid len <= capacity */
    bool is_null_buf;
    if (is_null_buf) {
        buf.buffer = NULL;
        buf.len = 0;
        buf.capacity = 0;
        buf.allocator = NULL;
    } else {
        size_t buf_capacity;
        __CPROVER_assume(buf_capacity <= MAX_BUFFER_SIZE);
        size_t buf_len;
        __CPROVER_assume(buf_len <= buf_capacity);
        
        uint8_t *nondet_buffer;
        /* buffer must be non-NULL if capacity > 0 */
        if (buf_capacity > 0) {
            __CPROVER_assume(nondet_buffer != NULL);
        }
        buf.buffer = nondet_buffer;
        buf.len = buf_len;
        buf.capacity = buf_capacity;
        buf.allocator = NULL;
    }
    
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* The function is a pure predicate — it should not modify ring_buffer or buf */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    assert(aws_ring_buffer_is_valid(&ring_buf));

    aws_ring_buffer_clean_up(&ring_buf);
}
