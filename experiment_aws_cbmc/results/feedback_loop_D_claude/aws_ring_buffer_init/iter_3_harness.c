#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <stdint.h>
#include <stdlib.h>

void aws_ring_buffer_init_harness(void) {
    /* Allocate and set up inputs */
    struct aws_ring_buffer ring_buf;
    
    /* Use the system allocator to ensure we have a valid allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    
    size_t size;
    /* Bound size to keep verification tractable */
    __CPROVER_assume(size > 0 && size <= 1024);

    /* Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* Return value must be either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* === SUCCESS PATH assertions === */
    if (result == AWS_OP_SUCCESS) {
        /* allocation must be non-NULL */
        assert(ring_buf.allocation != NULL);

        /* allocator must be set to the provided allocator */
        assert(ring_buf.allocator == allocator);

        /* allocation_end must be allocation + size */
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        /* head must be initialized to allocation */
        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        assert(head_ptr == ring_buf.allocation);

        /* tail must be initialized to allocation */
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(tail_ptr == ring_buf.allocation);

        /* head == tail means ring buffer is empty */
        assert(head_ptr == tail_ptr);

        /* allocation_end >= allocation (no overflow) */
        assert(ring_buf.allocation_end >= ring_buf.allocation);
        
        /* Clean up */
        aws_ring_buffer_clean_up(&ring_buf);
    }
}
