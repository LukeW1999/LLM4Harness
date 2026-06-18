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

    struct aws_allocator *allocator = aws_default_allocator();

    /* Use a small fixed-size backing buffer */
    size_t buf_size;
    __CPROVER_assume(buf_size >= 2 && buf_size <= MAX_BUFFER_SIZE);

    uint8_t *allocation = malloc(buf_size);
    __CPROVER_assume(allocation != NULL);

    ring_buf.allocator = allocator;
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + buf_size;

    /* Set head and tail to valid positions within the buffer */
    size_t head_offset;
    __CPROVER_assume(head_offset < buf_size);

    size_t tail_offset;
    __CPROVER_assume(tail_offset < buf_size);

    aws_atomic_init_ptr(&ring_buf.head, (void *)(allocation + head_offset));
    aws_atomic_init_ptr(&ring_buf.tail, (void *)(allocation + tail_offset));

    /* Only proceed if ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Set up destination byte buf */
    struct aws_byte_buf dest;
    dest.len = 0;
    dest.capacity = 0;
    dest.buffer = NULL;
    dest.allocator = NULL;

    /* requested_size must be > 0 and bounded */
    size_t requested_size;
    __CPROVER_assume(requested_size >= 1 && requested_size <= MAX_BUFFER_SIZE);

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
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
        assert(ring_buf.allocation == old_allocation);
        assert(ring_buf.allocation_end == old_allocation_end);
        assert(ring_buf.allocator == old_allocator);
        assert(aws_byte_buf_is_valid(&dest));
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        assert(ring_buf.allocation == old_allocation);
        assert(ring_buf.allocation_end == old_allocation_end);
        assert(ring_buf.allocator == old_allocator);
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }
}
