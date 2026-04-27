// === STEP 1: SUCCESS PATH ===
// When aws_ring_buffer_init returns AWS_OP_SUCCESS:
//   - ring_buf->allocation: CHANGES to a newly allocated buffer of `size` bytes (non-NULL)
//   - ring_buf->allocator: CHANGES to the provided allocator
//   - ring_buf->head: CHANGES to ring_buf->allocation (atomic ptr initialized)
//   - ring_buf->tail: CHANGES to ring_buf->allocation (atomic ptr initialized)
//   - ring_buf->allocation_end: CHANGES to ring_buf->allocation + size
//
// === STEP 2: FAILURE PATH ===
// When aws_ring_buffer_init returns AWS_OP_ERR (aws_mem_acquire fails):
//   - ring_buf->allocation: NULL (zeroed by AWS_ZERO_STRUCT, then allocation failed)
//   - ring_buf->allocator: NULL (zeroed by AWS_ZERO_STRUCT, not set on failure)
//   - ring_buf->head: zeroed (AWS_ZERO_STRUCT applied, not initialized)
//   - ring_buf->tail: zeroed (AWS_ZERO_STRUCT applied, not initialized)
//   - ring_buf->allocation_end: NULL (zeroed, not set on failure)
//
// === STEP 3: FRAME CONDITIONS ===
//   ring_buf (struct aws_ring_buffer *):
//     - allocation: CHANGED on success (non-NULL), NULL on failure
//     - allocator: CHANGED on success (set to allocator param), NULL on failure
//     - head: CHANGED on success (initialized to allocation ptr), zeroed on failure
//     - tail: CHANGED on success (initialized to allocation ptr), zeroed on failure
//     - allocation_end: CHANGED on success (allocation + size), NULL on failure
//   allocator (struct aws_allocator *):
//     - all fields: UNCHANGED always (not modified by this function)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_ring_buffer_is_valid(ring_buf): YES on success, NO on failure
//     (on success: allocation != NULL, head == tail == allocation,
//      allocation_end == allocation + size, allocator != NULL)

#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_ring_buffer_init_harness(void) {
    /* Allocate and set up inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = can_fail_allocator();
    size_t size;

    /* size must be > 0 for a meaningful ring buffer, but the function itself
       doesn't enforce this - we allow any size */
    __CPROVER_assume(allocator != NULL);

    /* Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

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

        /* validity invariant */
        assert(aws_ring_buffer_is_valid(&ring_buf));

        /* is_empty should hold since head == tail */
        assert(aws_ring_buffer_is_empty(&ring_buf));
    }

    /* === FAILURE PATH assertions === */
    if (result == AWS_OP_ERR) {
        /* On failure, AWS_ZERO_STRUCT was applied but allocation failed,
           so allocation should be NULL */
        assert(ring_buf.allocation == NULL);

        /* allocator not set on failure path */
        assert(ring_buf.allocator == NULL);

        /* allocation_end not set on failure path */
        assert(ring_buf.allocation_end == NULL);
    }

    /* === Return value must be either success or error === */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}
