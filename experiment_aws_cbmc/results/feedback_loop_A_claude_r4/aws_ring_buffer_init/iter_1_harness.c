#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness() {
    /* 1. Declare inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();

    /* Bound the size to keep the proof tractable */
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(allocator != NULL);

    /* 2. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 3. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - ring_buf.allocator is set to the provided allocator
         * - ring_buf.allocation is non-NULL and points to allocated memory of `size` bytes
         * - ring_buf.allocation_end == ring_buf.allocation + size
         * - head and tail atomics point to ring_buf.allocation (ring buffer is empty)
         */
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        /* head and tail should point to allocation (empty ring buffer) */
        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(head_ptr == ring_buf.allocation);
        assert(tail_ptr == ring_buf.allocation);

        /* Ring buffer should be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));

        /* Ring buffer should be empty */
        assert(aws_ring_buffer_is_empty(&ring_buf));

        /* Clean up to avoid memory leaks in the proof */
        aws_ring_buffer_clean_up(&ring_buf);
    } else {
        /* On failure:
         * - result must be AWS_OP_ERR
         * - allocation should be NULL (aws_mem_acquire failed)
         * - The struct was zeroed before the failed allocation
         */
        assert(result == AWS_OP_ERR);
        /* After AWS_ZERO_STRUCT and failed allocation, allocation is NULL */
        assert(ring_buf.allocation == NULL);
    }
}
