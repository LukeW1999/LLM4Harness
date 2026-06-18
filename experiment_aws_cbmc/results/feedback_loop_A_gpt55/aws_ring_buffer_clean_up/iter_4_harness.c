#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    const size_t allocation_size = 16;

    if (aws_ring_buffer_init(&ring_buf, allocator, allocation_size) == AWS_OP_SUCCESS) {
        assert(aws_ring_buffer_is_valid(&ring_buf));

        aws_ring_buffer_clean_up(&ring_buf);

        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }
}
