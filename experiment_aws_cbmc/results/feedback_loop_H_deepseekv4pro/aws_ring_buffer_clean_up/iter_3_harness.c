#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;

    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    /* Non-deterministic size for the backing store */
    size_t size;
    __CPROVER_assume(size > 0 && size <= 1024);

    /* Allocate memory through the same allocator */
    ring_buf.allocation = (uint8_t *)aws_mem_acquire(allocator, size);
    __CPROVER_assume(ring_buf.allocation != NULL);

    ring_buf.allocation_end = ring_buf.allocation + size;

    aws_ring_buffer_clean_up(&ring_buf);
}
