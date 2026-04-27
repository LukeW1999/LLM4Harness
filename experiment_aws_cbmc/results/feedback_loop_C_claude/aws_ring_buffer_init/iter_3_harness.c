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
    __CPROVER_assume(size > 0 && size <= 4096);

    /* Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* result must be either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* allocation must be non-NULL */
        assert(ring_buf.allocation != NULL);

        /* allocator must be set to the provided allocator */
        assert(ring_buf.allocator == allocator);

        /* allocation_end must be allocation + size */
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        /* head must point to allocation */
        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        assert(head_ptr == ring_buf.allocation);

        /* tail must point to allocation */
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(tail_ptr == ring_buf.allocation);

        /* ring buffer must be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));

        /* Clean up */
        aws_ring_buffer_clean_up(&ring_buf);

    } else {
        /* On failure, result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
    }
}
