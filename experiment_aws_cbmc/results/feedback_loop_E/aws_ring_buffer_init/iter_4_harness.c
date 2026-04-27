#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_init_harness() {
    /* Non-deterministic parameters. */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;

    /* Preconditions. */
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(size > 0 && size < MAX_MALLOC);

    /* Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buf = ring_buf;

    /* Operation under verification. */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* Postconditions. */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_ring_buffer_is_valid(&ring_buf));
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation_end - ring_buf.allocation == size);
        assert(AWS_MEM_IS_READABLE(ring_buf.allocation, size));
        assert(AWS_MEM_IS_WRITABLE(ring_buf.allocation, size));
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
    } else {
        assert(ring_buf.allocator == old_ring_buf.allocator);
        assert(ring_buf.allocation == old_ring_buf.allocation);
        assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
        assert(aws_atomic_load_ptr(&ring_buf.head) == aws_atomic_load_ptr(&old_ring_buf.head));
        assert(aws_atomic_load_ptr(&ring_buf.tail) == aws_atomic_load_ptr(&old_ring_buf.tail));
    }

    /* Fields that must NOT change regardless of result */
    assert(ring_buf.allocator == old_ring_buf.allocator);

    /* Validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
