#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    uint8_t buf[1024];
    size_t size;
    __CPROVER_assume(size > 0 && size <= 1024);

    ring_buf.allocation = buf;
    ring_buf.allocation_end = buf + size;
    ring_buf.head = buf;
    ring_buf.tail = buf;

    ring_buf.magic = AWS_RING_BUFFER_MAGIC;

    aws_ring_buffer_clean_up(&ring_buf);
}
