#include <assert.h>
#include <aws/common/allocator.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    ensure_ring_buffer_is_valid(&ring_buf);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_allocator *orig_allocator = ring_buf.allocator;
    uint8_t *orig_allocation = ring_buf.allocation;
    uint8_t *orig_allocation_end = ring_buf.allocation_end;

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocator == orig_allocator);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);
    assert(ring_buf.capacity == 0);
    assert(ring_buf.mask == 0);
}
