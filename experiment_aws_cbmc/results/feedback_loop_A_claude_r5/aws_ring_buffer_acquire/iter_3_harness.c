#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;

    size_t buffer_size;
    __CPROVER_assume(buffer_size >= 2 && buffer_size <= MAX_BUFFER_SIZE);

    uint8_t *allocation = malloc(buffer_size);
    __CPROVER_assume(allocation != NULL);

    ring_buf.allocator = aws_default_allocator();
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + buffer_size;

    /* Initialize head and tail to the start of the buffer (empty ring buffer) */
    aws_atomic_init_ptr(&ring_buf.head, (void *)allocation);
    aws_atomic_init_ptr(&ring_buf.tail, (void *)allocation);

    /* Verify the ring buffer is valid before calling */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    size_t requested_size;
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size < buffer_size);

    struct aws_byte_buf dest;
    /* Initialize dest to known state */
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;
    dest.allocator = NULL;

    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Fields that must never change */
    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.len == 0);
        assert(dest.capacity == requested_size);
        assert(dest.buffer != NULL);

        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);

        assert(aws_ring_buffer_is_valid(&ring_buf));
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    } else {
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }

    assert(aws_ring_buffer_is_valid(&ring_buf));
}
