#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;

    if (nondet_bool()) {
        struct aws_allocator *allocator = aws_default_allocator();
        size_t size = nondet_size_t();

        __CPROVER_assume(allocator != NULL);
        __CPROVER_assume(size > 0);
        __CPROVER_assume(size <= MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_ring_buffer_init(&ring_buf, allocator, size) == AWS_OP_SUCCESS);
    } else {
        AWS_ZERO_STRUCT(ring_buf);
    }

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
    assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
