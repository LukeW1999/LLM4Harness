#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness() {
    /* Non-deterministic parameters. */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Preconditions. */
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    __CPROVER_assume(AWS_MEM_IS_READABLE(ring_buf.allocation, ring_buf.allocation_end - ring_buf.allocation));
    __CPROVER_assume(ring_buf.allocator == allocator);
    __CPROVER_assume(allocator->mem_release != NULL);

    /* Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old = ring_buf;

    /* Operation under verification. */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Postconditions. */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocator == old.allocator);
    assert(ring_buf.allocation_end == NULL);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
