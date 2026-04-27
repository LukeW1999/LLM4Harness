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

    /* Assume allocator is non-NULL to avoid null dereference before allocation */
    __CPROVER_assume(allocator != NULL);
    /* Assume size is reasonable to avoid issues */
    __CPROVER_assume(size > 0 && size <= 4096);

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

        /* Clean up to avoid memory leaks in the proof */
        aws_ring_buffer_clean_up(&ring_buf);

    } else {
        /* === FAILURE PATH assertions === */
        assert(result == AWS_OP_ERR);

        /* After AWS_ZERO_STRUCT and failed allocation, allocation should be NULL */
        assert(ring_buf.allocation == NULL);

        /* allocator should be NULL (zeroed, since we zero before setting) */
        assert(ring_buf.allocator == NULL);

        /* allocation_end should be NULL */
        assert(ring_buf.allocation_end == NULL);
    }
}
