#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void aws_ring_buffer_init_harness() {
    struct aws_ring_buffer ring_buf;

    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    size_t size;
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* The result must be either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* On success: allocation must be non-null */
        assert(ring_buf.allocation != NULL);

        /* allocator must be set to the provided allocator */
        assert(ring_buf.allocator == allocator);

        /* allocation_end must be allocation + size */
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        /* allocation_end must be strictly greater than allocation (size > 0) */
        assert(ring_buf.allocation_end > ring_buf.allocation);

        /* head and tail must point to allocation (ring buffer is empty after init) */
        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(head_ptr == ring_buf.allocation);
        assert(tail_ptr == ring_buf.allocation);

        /* Clean up */
        aws_ring_buffer_clean_up(&ring_buf);
    } else {
        /* On failure: result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
    }
}
