#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void aws_ring_buffer_acquire_harness() {
    struct aws_ring_buffer ring_buf;

    /* Use a small fixed-size backing buffer */
    size_t buf_size;
    __CPROVER_assume(buf_size >= 2 && buf_size <= MAX_BUFFER_SIZE);

    uint8_t *allocation = malloc(buf_size);
    __CPROVER_assume(allocation != NULL);

    ring_buf.allocator = aws_default_allocator();
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + buf_size;

    /* head and tail must be within [allocation, allocation_end] */
    size_t head_offset;
    __CPROVER_assume(head_offset <= buf_size);
    uint8_t *head_ptr = allocation + head_offset;

    size_t tail_offset;
    __CPROVER_assume(tail_offset <= buf_size);
    uint8_t *tail_ptr = allocation + tail_offset;

    aws_atomic_init_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_init_ptr(&ring_buf.tail, (void *)tail_ptr);

    /* Only proceed if the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* requested_size must be > 0 and bounded */
    size_t requested_size;
    __CPROVER_assume(requested_size >= 1 && requested_size <= MAX_BUFFER_SIZE);

    /* Destination byte buf */
    struct aws_byte_buf dest;
    dest.allocator = NULL;
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;

    /* Save old state for comparison */
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    struct aws_allocator *old_allocator = ring_buf.allocator;

    /* Call the function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Check postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
        assert(dest.buffer != NULL);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        assert(ring_buf.allocator == old_allocator);
        assert(ring_buf.allocation == old_allocation);
        assert(ring_buf.allocation_end == old_allocation_end);
        assert(aws_ring_buffer_is_valid(&ring_buf));
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    } else {
        assert(ring_buf.allocator == old_allocator);
        assert(ring_buf.allocation == old_allocation);
        assert(ring_buf.allocation_end == old_allocation_end);
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }
}
