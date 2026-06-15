#include <aws/common/ring_buffer.h>
#include <aws/common/ring_buffer.c>  // Provide implementation for CBMC
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;

    /* Initialize a valid ring buffer with a non‑null allocator */
    ring_buf.allocator = can_fail_allocator();
    __CPROVER_assume(ring_buf.allocator != NULL);

    size_t size;
    __CPROVER_assume(size > 0);
    uint8_t *allocation = malloc(size);
    __CPROVER_assume(allocation != NULL);
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + size;

    aws_atomic_store_ptr(&ring_buf.head, allocation);
    aws_atomic_store_ptr(&ring_buf.tail, allocation);

    aws_ring_buffer_clean_up(&ring_buf);

    /* Post‑condition: the entire struct should be zeroed */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
    assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
}
