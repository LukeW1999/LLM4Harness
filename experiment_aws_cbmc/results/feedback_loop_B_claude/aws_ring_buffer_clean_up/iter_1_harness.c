#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * aws_ring_buffer_clean_up implementation:
 *
 *   void aws_ring_buffer_clean_up(struct aws_ring_buffer *ring_buf) {
 *       if (ring_buf->allocation) {
 *           aws_mem_release(ring_buf->allocator, ring_buf->allocation);
 *       }
 *       AWS_ZERO_STRUCT(*ring_buf);
 *   }
 *
 * Analysis:
 * 1. Changed fields: ALL fields are zeroed via AWS_ZERO_STRUCT after the call.
 *    - ring_buf->allocator == NULL
 *    - ring_buf->allocation == NULL
 *    - ring_buf->allocation_end == NULL
 *    - ring_buf->head (atomic) zeroed
 *    - ring_buf->tail (atomic) zeroed
 * 2. Unchanged fields: none — everything is zeroed.
 * 3. Failure path: function is void, no failure path.
 * 4. Validity: after clean_up, the struct is zeroed (not "valid" in the
 *    aws_ring_buffer_is_valid sense, but all pointers are NULL).
 */

void aws_ring_buffer_clean_up_harness(void) {
    /* 1. Declare the ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Set up a valid allocator non-deterministically */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Non-deterministically decide if allocation is NULL or non-NULL */
    bool has_allocation = nondet_bool();

    if (has_allocation) {
        /* Allocate a small buffer so aws_mem_release has something valid to free */
        size_t alloc_size = nondet_size_t();
        __CPROVER_assume(alloc_size > 0 && alloc_size <= 4096);

        ring_buf.allocator = allocator;
        ring_buf.allocation = (uint8_t *)aws_mem_acquire(allocator, alloc_size);
        __CPROVER_assume(ring_buf.allocation != NULL);
        ring_buf.allocation_end = ring_buf.allocation + alloc_size;
    } else {
        ring_buf.allocator = allocator;
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
    }

    /* Initialize atomics to some non-deterministic values */
    aws_atomic_init_ptr(&ring_buf.head, (void *)ring_buf.allocation);
    aws_atomic_init_ptr(&ring_buf.tail, (void *)ring_buf.allocation);

    /* 2. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 3. Assert postconditions: AWS_ZERO_STRUCT zeroes everything */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);

    /* The atomic variables should be zeroed as well (they are stored inline) */
    /* AWS_ZERO_STRUCT zeros the entire struct including the atomic vars */
    /* We check the underlying storage is zero by loading the atomic values */
    void *head_val = aws_atomic_load_ptr(&ring_buf.head);
    void *tail_val = aws_atomic_load_ptr(&ring_buf.tail);
    assert(head_val == NULL);
    assert(tail_val == NULL);
}
