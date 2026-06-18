#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_init_harness() {
    /* 1. Declare inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Bound the size to prevent state space explosion */
    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    /* 2. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 3. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: allocation is non-null */
        assert(ring_buf.allocation != NULL);
        /* allocator is set correctly */
        assert(ring_buf.allocator == allocator);
        /* allocation_end = allocation + size */
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        /* head and tail point to allocation start */
        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(head_ptr == ring_buf.allocation);
        assert(tail_ptr == ring_buf.allocation);
        /* The ring buffer is valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));
        /* The ring buffer is empty after init */
        assert(aws_ring_buffer_is_empty(&ring_buf));
        /* allocation_end > allocation (since size > 0) */
        assert(ring_buf.allocation_end > ring_buf.allocation);
    } else {
        /* On failure: result should be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
    }
}
