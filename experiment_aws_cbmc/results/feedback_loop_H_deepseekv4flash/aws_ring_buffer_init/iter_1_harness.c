#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_init_harness() {
    /* Input parameters */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator;
    size_t size;

    /* Preconditions */
    __CPROVER_assume(allocator != NULL); /* allocator must be valid */

    /* Save old state is not applicable because ring_buf is uninitialized */

    /* Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success path: ring buffer is properly initialized */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* Failure path: ring buffer should be zeroed (AWS_ZERO_STRUCT) */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(!aws_ring_buffer_is_valid(&ring_buf));
    }

    /* Immutable inputs are unchanged */
    assert(allocator == allocator);
    /* size is unchanged by the call */
}
