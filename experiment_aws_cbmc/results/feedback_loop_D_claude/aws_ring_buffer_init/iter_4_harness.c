#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

void aws_ring_buffer_init_harness(void) {
    struct aws_ring_buffer ring_buf;
    
    struct aws_allocator *allocator = aws_default_allocator();
    
    size_t size;
    __CPROVER_assume(size > 0 && size <= 64);

    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* Basic postcondition: result is either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        
        assert(head_ptr == ring_buf.allocation);
        assert(tail_ptr == ring_buf.allocation);
        assert(head_ptr == tail_ptr);
        assert(ring_buf.allocation_end >= ring_buf.allocation);
        
        aws_ring_buffer_clean_up(&ring_buf);
    } else {
        /* On failure, verify error code is set */
        assert(aws_last_error() != 0);
    }
}

int main(void) {
    aws_ring_buffer_init_harness();
    return 0;
}
