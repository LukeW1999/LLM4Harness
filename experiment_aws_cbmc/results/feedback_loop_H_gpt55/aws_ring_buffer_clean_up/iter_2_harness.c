#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    if (nondet_bool()) {
        size_t size = nondet_size_t();
        __CPROVER_assume(size > 0);
        __CPROVER_assume(size <= MAX_BUFFER_SIZE);

        int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
        __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    } else {
        AWS_ZERO_STRUCT(ring_buf);
    }

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocator == NULL);
    assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
    assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_ring_buffer_is_empty(&ring_buf));
}
