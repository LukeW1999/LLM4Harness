#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 32
#endif

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;

    size_t buffer_size;
    __CPROVER_assume(buffer_size > 0 && buffer_size <= MAX_BUFFER_SIZE);

    uint8_t *allocation = malloc(buffer_size);
    __CPROVER_assume(allocation != NULL);

    struct aws_allocator *allocator = malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(allocator != NULL);

    ring_buf.allocator = allocator;
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + buffer_size;

    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= buffer_size);
    __CPROVER_assume(tail_offset <= buffer_size);

    uint8_t *head_ptr = allocation + head_offset;
    uint8_t *tail_ptr = allocation + tail_offset;

    aws_atomic_init_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_init_ptr(&ring_buf.tail, (void *)tail_ptr);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    size_t requested_size;
    __CPROVER_assume(requested_size > 0 && requested_size <= buffer_size);

    struct aws_byte_buf dest;
    dest.len = 0;
    dest.capacity = 0;
    dest.buffer = NULL;
    dest.allocator = NULL;

    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
        assert(dest.buffer != NULL);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        assert(aws_ring_buffer_is_valid(&ring_buf));
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    } else {
        assert(result == AWS_OP_ERR);
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }
}
