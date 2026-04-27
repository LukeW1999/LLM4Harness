#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;
    ring_buf.allocation = (uint8_t *)bounded_malloc(sizeof(uint8_t) * 100); // Allocate some memory
    ring_buf.head = (struct aws_atomic_var){ .var = 0 };
    ring_buf.tail = (struct aws_atomic_var){ .var = 0 };
    ring_buf.allocation_end = ring_buf.allocation + 100;

    // Initialize the ring buffer to ensure it's valid before cleanup
    aws_ring_buffer_init(&ring_buf, allocator, 100);

    // Capture the state before cleanup
    struct aws_ring_buffer old_ring_buf = ring_buf;

    // Call the function under test
    aws_ring_buffer_clean_up(&ring_buf);

    // Assertions for success path
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.head.var == 0);
    assert(ring_buf.tail.var == 0);
    assert(ring_buf.allocation_end == NULL);

    // Validity invariant
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
