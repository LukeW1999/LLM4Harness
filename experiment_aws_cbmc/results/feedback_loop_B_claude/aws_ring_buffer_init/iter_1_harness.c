#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_ring_buffer_init_harness(void) {
    /* 1. Declare inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = can_fail_allocator();
    size_t size;

    /* Bound the size to keep the state space manageable */
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* 2. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 3. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - ring_buf.allocator is set to the provided allocator
         * - ring_buf.allocation is non-NULL (memory was acquired)
         * - ring_buf.allocation_end == ring_buf.allocation + size
         * - head and tail atomics are initialized to ring_buf.allocation
         * - ring_buf is valid
         */
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        /* head and tail should point to allocation */
        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(head_ptr == ring_buf.allocation);
        assert(tail_ptr == ring_buf.allocation);

        /* Validity invariant */
        assert(aws_ring_buffer_is_valid(&ring_buf));

        /* Ring buffer should be empty after init */
        assert(aws_ring_buffer_is_empty(&ring_buf));
    } else {
        /* On failure (AWS_OP_ERR):
         * - aws_mem_acquire returned NULL
         * - ring_buf was zeroed (AWS_ZERO_STRUCT was called)
         * - ring_buf.allocation is NULL (zeroed and mem_acquire failed)
         * - ring_buf.allocator is NULL (zeroed, not set because allocation failed)
         * - ring_buf.allocation_end is NULL (zeroed)
         */
        assert(result == AWS_OP_ERR);
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation_end == NULL);
    }
}
