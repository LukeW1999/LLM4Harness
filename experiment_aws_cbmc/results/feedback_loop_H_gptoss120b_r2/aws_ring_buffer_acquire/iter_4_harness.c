#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(alloc_size > 0);
    ring_buf.allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;
    ring_buf.allocator = aws_default_allocator();

    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= alloc_size);
    __CPROVER_assume(tail_offset <= alloc_size);
    uint8_t *head_ptr = ring_buf.allocation + head_offset;
    uint8_t *tail_ptr = ring_buf.allocation + tail_offset;
    ring_buf.head = (struct aws_atomic_var){ .value = (void *)head_ptr };
    ring_buf.tail = (struct aws_atomic_var){ .value = (void *)tail_ptr };

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_byte_buf dest = {0};

    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_dest = dest;

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.len <= ring_buf.allocation_end);
        /* The length may be less than the requested size if the buffer wraps. */
        assert(dest.len > 0);
        assert(dest.len <= requested_size);
        assert(dest.capacity >= dest.len);
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
    } else {
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        assert(ring_buf.head.value == old_ring.head.value);
        assert(ring_buf.tail.value == old_ring.tail.value);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
    }

    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);
    assert(aws_ring_buffer_is_valid(&ring_buf));

    free(ring_buf.allocation);
}
