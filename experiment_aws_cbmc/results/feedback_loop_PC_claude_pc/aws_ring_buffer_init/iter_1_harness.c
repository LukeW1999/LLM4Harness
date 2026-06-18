#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness(void) {
    /* 1. Declare inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();

    /* Bound the size to keep the state space manageable */
    __CPROVER_assume(size > 0 && size <= 4096);

    /* 2. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 3. Assert postconditions */

    /* Both return paths */
    if (result == AWS_OP_SUCCESS) {
        /* On success: allocation must be non-null */
        assert(ring_buf.allocation != NULL);

        /* allocator field must be set to the provided allocator */
        assert(ring_buf.allocator == allocator);

        /* allocation_end must be allocation + size */
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        /* head and tail must point to allocation (start) */
        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(head_ptr == ring_buf.allocation);
        assert(tail_ptr == ring_buf.allocation);

        /* allocation_end > allocation (no overflow) */
        assert(ring_buf.allocation_end > ring_buf.allocation);

        /* The ring buffer must be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));

        /* The ring buffer must be empty after initialization */
        assert(aws_ring_buffer_is_empty(&ring_buf));

        /* head and tail are within bounds */
        assert(aws_ring_buffer_check_atomic_ptr(&ring_buf, head_ptr));
        assert(aws_ring_buffer_check_atomic_ptr(&ring_buf, tail_ptr));

        /* Clean up to avoid memory leaks in the model */
        aws_ring_buffer_clean_up(&ring_buf);
    } else {
        /* On failure: result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);

        /* On failure, allocation should be NULL (AWS_ZERO_STRUCT was called,
           and aws_mem_acquire returned NULL) */
        assert(ring_buf.allocation == NULL);

        /* allocator should be NULL since we zero-struct'd and didn't set it */
        assert(ring_buf.allocator == NULL);

        /* allocation_end should be NULL */
        assert(ring_buf.allocation_end == NULL);
    }
}
