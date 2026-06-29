#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;

    /* Precondition: use a valid allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Choose a non-deterministic size for the allocation, bounded for tractability */
    size_t size;
    __CPROVER_assume(size > 0 && size <= 1024);

    /* Initialize the ring buffer properly so it is valid */
    int rc = aws_ring_buffer_init(&ring_buf, allocator, size);
    /* Only proceed if initialization succeeded */
    __CPROVER_assume(rc == AWS_OP_SUCCESS);

    /* Precondition: ring buffer must be valid before calling clean_up */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Postconditions: struct is fully zeroed */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
}
