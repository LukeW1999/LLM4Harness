// === STEP 1: SUCCESS PATH ===
// When aws_ring_buffer_init returns AWS_OP_SUCCESS:
//   - ring_buf->allocation: CHANGES to a newly allocated buffer of `size` bytes
//   - ring_buf->allocator: CHANGES to the provided allocator
//   - ring_buf->head: CHANGES to point to ring_buf->allocation
//   - ring_buf->tail: CHANGES to point to ring_buf->allocation
//   - ring_buf->allocation_end: CHANGES to ring_buf->allocation + size
//
// === STEP 2: FAILURE PATH ===
// When aws_ring_buffer_init returns AWS_OP_ERR (aws_mem_acquire fails):
//   - ring_buf->allocation: NULL (zeroed by AWS_ZERO_STRUCT)
//   - ring_buf->allocator: NULL (zeroed by AWS_ZERO_STRUCT)
//   - ring_buf->head: zeroed
//   - ring_buf->tail: zeroed
//   - ring_buf->allocation_end: NULL (zeroed by AWS_ZERO_STRUCT)
//
// === STEP 3: FRAME CONDITIONS ===
//   ring_buf (struct aws_ring_buffer):
//     - allocation: CHANGED on success (non-NULL), zeroed on failure
//     - allocator: CHANGED on success (= allocator param), zeroed on failure
//     - head: CHANGED on success (= allocation), zeroed on failure
//     - tail: CHANGED on success (= allocation), zeroed on failure
//     - allocation_end: CHANGED on success (= allocation + size), zeroed on failure
//   allocator (struct aws_allocator*):
//     - all fields: UNCHANGED always (not modified by this function)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_ring_buffer_is_valid(&ring_buf): YES on success, not necessarily on failure
//   - On success: allocation != NULL, allocation_end == allocation + size,
//     head and tail both point to allocation (ring buffer is empty)

#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_ring_buffer_init_harness(void) {
    /* Allocate and set up inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = can_fail_allocator();
    size_t size;

    /* Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    if (result == AWS_OP_SUCCESS) {
        /* === SUCCESS PATH assertions === */

        /* allocation must be non-NULL */
        assert(ring_buf.allocation != NULL);

        /* allocator must be set to the provided allocator */
        assert(ring_buf.allocator == allocator);

        /* allocation_end must be allocation + size */
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        /* head must point to allocation */
        uint8_t *head_ptr = aws_atomic_load_ptr(&ring_buf.head);
        assert(head_ptr == ring_buf.allocation);

        /* tail must point to allocation */
        uint8_t *tail_ptr = aws_atomic_load_ptr(&ring_buf.tail);
        assert(tail_ptr == ring_buf.allocation);

        /* ring buffer must be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));

        /* ring buffer must be empty (head == tail) */
        assert(aws_ring_buffer_is_empty(&ring_buf));

        /* Clean up to avoid memory leaks in the proof */
        aws_ring_buffer_clean_up(&ring_buf);

    } else {
        /* === FAILURE PATH assertions === */
        assert(result == AWS_OP_ERR);

        /* After AWS_ZERO_STRUCT and failed allocation, allocation should be NULL */
        assert(ring_buf.allocation == NULL);

        /* allocator should be NULL (zeroed, since we zero before setting) */
        /* Note: AWS_ZERO_STRUCT zeros everything first, then on failure we return early */
        assert(ring_buf.allocator == NULL);

        /* allocation_end should be NULL */
        assert(ring_buf.allocation_end == NULL);
    }
}
