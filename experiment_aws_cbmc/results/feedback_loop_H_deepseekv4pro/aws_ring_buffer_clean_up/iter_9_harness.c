#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    ensure_ring_buffer_has_allocated_members(&ring_buf, 1024);

    aws_ring_buffer_clean_up(&ring_buf);
}
