#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_ring_buffer_init_harness() {
    /* 1. Declare inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;

    /* Bound the size to keep state space manageable */
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* Assume allocator is valid */
    __CPROVER_assume(allocator != NULL);

    /* 2. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 3. Assert postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* On success: allocation must be non-null */
        assert(ring_buf.allocation != NULL);

        /* allocator must be set to the provided allocator */
        assert(ring_buf.allocator == allocator);

        /* allocation_end must be allocation + size */
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        /* head and tail must point to allocation (ring buffer is empty after init) */
        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(head_ptr == ring_buf.allocation);
        assert(tail_ptr == ring_buf.allocation);

        /* allocation_end must be strictly greater than allocation (size > 0) */
        assert(ring_buf.allocation_end > ring_buf.allocation);

        /* Clean up to avoid memory leaks in proof */
        aws_ring_buffer_clean_up(&ring_buf);
    } else {
        /* On failure: result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);

        /* On failure, allocation should be NULL (zeroed then failed) */
        assert(ring_buf.allocation == NULL);

        /* On failure, allocator should be NULL (zeroed, then allocation failed before setting) */
        assert(ring_buf.allocator == NULL);

        /* On failure, allocation_end should be NULL */
        assert(ring_buf.allocation_end == NULL);
    }
}
