#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer *ring_buf = malloc(sizeof(struct aws_ring_buffer));
    __CPROVER_assume(ring_buf != NULL);

    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();
    /* Bound size to avoid excessive memory allocation */
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* 2. Save old state BEFORE calling — ring_buf is uninitialized, so we save nothing meaningful.
     *    We will check that on failure, ring_buf is zeroed (as per AWS_ZERO_STRUCT). */

    /* 3. Call function under test */
    int result = aws_ring_buffer_init(ring_buf, allocator, size);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: allocation must be non-NULL */
        assert(ring_buf->allocation != NULL);
        /* allocator must be set */
        assert(ring_buf->allocator == allocator);
        /* allocation_end must be allocation + size */
        assert(ring_buf->allocation_end == ring_buf->allocation + size);
        /* head and tail must point to allocation */
        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf->head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf->tail);
        assert(head_ptr == ring_buf->allocation);
        assert(tail_ptr == ring_buf->allocation);
        /* allocation must be writable for size bytes */
        assert(AWS_MEM_IS_WRITABLE(ring_buf->allocation, size));
    } else {
        /* Failure: aws_mem_acquire returned NULL, so allocation is NULL */
        assert(ring_buf->allocation == NULL);
        /* AWS_ZERO_STRUCT was called, so all fields should be zero */
        assert(ring_buf->allocator == NULL);
        assert(ring_buf->allocation_end == NULL);
        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf->head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf->tail);
        assert(head_ptr == NULL);
        assert(tail_ptr == NULL);
    }

    /* 5. Assert validity invariant always holds */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_ring_buffer_is_valid(ring_buf));
    }

    /* 6. Clean up */
    if (result == AWS_OP_SUCCESS) {
        aws_ring_buffer_clean_up(ring_buf);
    }
    free(ring_buf);
}
