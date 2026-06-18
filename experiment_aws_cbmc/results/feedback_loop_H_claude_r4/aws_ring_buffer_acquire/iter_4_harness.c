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

    size_t buffer_size;
    __CPROVER_assume(buffer_size >= 1 && buffer_size <= MAX_BUFFER_SIZE);

    int init_result = aws_ring_buffer_init(&ring_buf, allocator, buffer_size);
    if (init_result != AWS_OP_SUCCESS) {
        return;
    }

    /* Set nondeterministic head and tail within valid range */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset < buffer_size);
    __CPROVER_assume(tail_offset < buffer_size);

    uint8_t *head_ptr = ring_buf.allocation + head_offset;
    uint8_t *tail_ptr = ring_buf.allocation + tail_offset;

    aws_atomic_store_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_store_ptr(&ring_buf.tail, (void *)tail_ptr);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    size_t requested_size;
    __CPROVER_assume(requested_size >= 1 && requested_size <= buffer_size);

    struct aws_byte_buf dest;
    dest.len = 0;
    dest.capacity = 0;
    dest.buffer = NULL;
    dest.allocator = NULL;

    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    struct aws_allocator *old_allocator = ring_buf.allocator;

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Invariant: these fields never change */
    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);

    /* Ring buffer must always be valid after the call */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == 0);
        assert(dest.capacity == requested_size);
        assert(dest.buffer != NULL);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        assert(aws_byte_buf_is_valid(&dest));
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    }

    aws_ring_buffer_clean_up(&ring_buf);
}
