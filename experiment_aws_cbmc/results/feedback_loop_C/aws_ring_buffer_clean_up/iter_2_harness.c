#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer old_ring_buf = ring_buf;

    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;
    ring_buf.allocation = (uint8_t *)nondet_ptr();
    ring_buf.head = (struct aws_atomic_var){ .var = nondet_uint64_t() };
    ring_buf.tail = (struct aws_atomic_var){ .var = nondet_uint64_t() };
    ring_buf.allocation_end = (uint8_t *)nondet_ptr();

    aws_ring_buffer_clean_up(&ring_buf);

    // Assertions for success path
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert((ring_buf.allocation == NULL && ring_buf.head.var == 0 && ring_buf.tail.var == 0 && ring_buf.allocation_end == NULL) ||
           (ring_buf.allocation == old_ring_buf.allocation && ring_buf.head.var == old_ring_buf.head.var && ring_buf.tail.var == old_ring_buf.tail.var && ring_buf.allocation_end == old_ring_buf.allocation_end));

    // Validity invariant
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
