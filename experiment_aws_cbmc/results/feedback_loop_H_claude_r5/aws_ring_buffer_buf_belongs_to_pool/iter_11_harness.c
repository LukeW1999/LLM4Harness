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
    if (aws_ring_buffer_init(&ring_buf, allocator, size) != AWS_OP_SUCCESS) {
        return;
    }
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Set up byte buf that satisfies aws_byte_buf_is_valid */
    struct aws_byte_buf buf;
    
    /* Use ensure_byte_buf_has_allocated_buffer_member or set up manually */
    /* aws_byte_buf_is_valid requires: buffer != NULL if capacity > 0, 
       len <= capacity, and allocator can be anything */
    
    size_t buf_capacity;
    size_t buf_len;
    
    __CPROVER_assume(buf_capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf_len <= buf_capacity);
    
    buf.capacity = buf_capacity;
    buf.len = buf_len;
    buf.allocator = NULL;
    
    if (buf_capacity > 0) {
        uint8_t *buf_ptr = malloc(buf_capacity);
        __CPROVER_assume(buf_ptr != NULL);
        buf.buffer = buf_ptr;
    } else {
        buf.buffer = NULL;
    }
    
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* Check that ring_buffer fields are unchanged */
    __CPROVER_assert(ring_buf.allocator == allocator, "allocator unchanged");
    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buf), "ring buffer valid");

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
}
