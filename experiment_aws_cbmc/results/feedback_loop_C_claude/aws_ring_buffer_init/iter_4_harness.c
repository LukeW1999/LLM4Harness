#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <stdint.h>
#include <stdlib.h>

void aws_ring_buffer_init_harness(void) {
    /* Allocate and set up inputs */
    struct aws_ring_buffer ring_buf;
    size_t size;

    /* Use a real allocator that can fail */
    struct aws_allocator *allocator = can_fail_allocator();

    /* Assume allocator is non-NULL */
    __CPROVER_assume(allocator != NULL);

    /* Assume size is bounded to keep proof tractable */
    __CPROVER_assume(size > 0 && size <= 16);

    /* Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* result must be either success or error - this is always reachable */
    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR,
                     "result is either success or error");

    if (result == AWS_OP_SUCCESS) {
        /* allocation must be non-NULL */
        __CPROVER_assert(ring_buf.allocation != NULL,
                         "allocation is non-NULL on success");

        /* allocator must be set to the provided allocator */
        __CPROVER_assert(ring_buf.allocator == allocator,
                         "allocator is set correctly");

        /* allocation_end must be allocation + size */
        __CPROVER_assert(ring_buf.allocation_end == ring_buf.allocation + size,
                         "allocation_end is allocation + size");

        /* head must point to allocation */
        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        __CPROVER_assert(head_ptr == ring_buf.allocation,
                         "head points to allocation");

        /* tail must point to allocation */
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        __CPROVER_assert(tail_ptr == ring_buf.allocation,
                         "tail points to allocation");

        /* ring buffer must be valid */
        __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buf),
                         "ring buffer is valid after init");

        /* Clean up */
        aws_ring_buffer_clean_up(&ring_buf);

    } else {
        /* On failure, result must be AWS_OP_ERR */
        __CPROVER_assert(result == AWS_OP_ERR,
                         "result is AWS_OP_ERR on failure");
    }
}
