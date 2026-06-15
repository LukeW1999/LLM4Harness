#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_ring_buffer_init_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t size = nondet_size_t();
    /* Optionally bound size to avoid huge allocations */
    __CPROVER_assume(size <= 1024);

    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    if (result == AWS_OP_SUCCESS) {
        /* Allocation must have succeeded */
        assert(ring_buf.allocation != NULL);
        /* Allocator must be the one passed in */
        assert(ring_buf.allocator == allocator);
        /* Head and tail must point to the start of the allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
        /* allocation_end must be correctly set */
        assert(ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size);
        /* Return value must be success */
        assert(result == AWS_OP_SUCCESS);
        /* The ring buffer must be in a valid state */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* Allocation must have failed */
        assert(ring_buf.allocation == NULL);
        /* Allocator must remain NULL */
        assert(ring_buf.allocator == NULL);
        /* Head and tail must be NULL */
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
        /* allocation_end must be NULL */
        assert(ring_buf.allocation_end == NULL);
        /* Return value must be error */
        assert(result == AWS_OP_ERR);
        /* The ring buffer is not valid when allocation fails */
        assert(!aws_ring_buffer_is_valid(&ring_buf));
    }
}
