#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 16
#endif

void aws_ring_buffer_acquire_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Use a fixed small buffer size to keep CBMC tractable */
    size_t buf_size;
    __CPROVER_assume(buf_size >= 2 && buf_size <= MAX_BUFFER_SIZE);

    uint8_t *allocation = (uint8_t *)malloc(buf_size);
    __CPROVER_assume(allocation != NULL);

    ring_buf.allocator = allocator;
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + buf_size;

    /* head and tail must be within [allocation, allocation_end] */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= buf_size);
    __CPROVER_assume(tail_offset <= buf_size);

    aws_atomic_init_ptr(&ring_buf.head, (void *)(allocation + head_offset));
    aws_atomic_init_ptr(&ring_buf.tail, (void *)(allocation + tail_offset));

    /* Ensure the ring buffer is valid before calling */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* requested_size must be > 0 and <= buf_size */
    size_t requested_size;
    __CPROVER_assume(requested_size > 0 && requested_size <= buf_size);

    /* Save state before call */
    uint8_t *old_allocation     = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    struct aws_allocator *old_allocator = ring_buf.allocator;

    struct aws_byte_buf dest;
    dest.buffer    = NULL;
    dest.len       = 0;
    dest.capacity  = 0;
    dest.allocator = NULL;

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Immutable fields must not change */
    assert(ring_buf.allocator      == old_allocator);
    assert(ring_buf.allocation     == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);

    /* Ring buffer must still be valid */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid, non-null buffer of exactly requested_size */
        assert(dest.buffer   != NULL);
        assert(dest.len      == 0);
        assert(dest.capacity == requested_size);

        /* dest.buffer must lie inside the ring buffer allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);

        /* dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* The acquired buffer must belong to this ring buffer */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    }
}
